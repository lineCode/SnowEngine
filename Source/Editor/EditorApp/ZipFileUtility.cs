// ZipFileUtility.cs - creates a ZIP file readable by the GameCode4 engine

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.IO.Packaging;
using System.Xml;

namespace EditorApp
{
    //----------------------------------------------------------------
    // ZipFileUtility                       - 
    //----------------------------------------------------------------

    class ZipFileUtility
    {
        public static void Create(string rootDirectoryName, string zipFileName)
        {
            DirectoryInfo rootDirectory = new DirectoryInfo(rootDirectoryName);
            int rootDirLen = rootDirectory.FullName.Length;

            using (Package package = ZipPackage.Open(zipFileName, FileMode.Create))
            {
                var stack = new Stack<string>();
                stack.Push(rootDirectory.FullName);

                while (stack.Count > 0)
                {
                    var currentNode = stack.Pop();
                    var directoryInfo = new DirectoryInfo(currentNode);
                    foreach (var directory in directoryInfo.GetDirectories())
                    {
                        FileAttributes attributes = File.GetAttributes(directory.FullName);
                        if ((attributes & FileAttributes.Hidden) == 0 && directory.Name != "Editor")
                        {
                            stack.Push(directory.FullName);
                        }
                    }
                    foreach (var file in directoryInfo.GetFiles())
                    {
                        FileAttributes attributes = File.GetAttributes(file.FullName);
                        if ((attributes & FileAttributes.Hidden) == 0)
                        {
                            Console.WriteLine("Packing " + file.FullName);
                            string relativeFromRoot = file.FullName.Substring(rootDirLen);
                            Uri relUri = GetRelativeUri(relativeFromRoot);

                            PackagePart packagePart =
                                package.CreatePart(relUri, System.Net.Mime.MediaTypeNames.Application.Octet, CompressionOption.Maximum);
                            using (FileStream fileStream = new FileStream(file.FullName, FileMode.Open, FileAccess.Read))
                            {
                                CopyStream(fileStream, packagePart.GetStream());
                            }
                        }
                    }
                }
            }
        }

        private static void CopyStream(Stream source, Stream target)
        {
            const int bufSize = 16384;
            byte[] buf = new byte[bufSize];
            int bytesRead = 0;
            while ((bytesRead = source.Read(buf, 0, bufSize)) > 0)
                target.Write(buf, 0, bytesRead);
        }

        private static Uri GetRelativeUri(string currentFile)
        {
            string pastBackslashes = currentFile.Substring(currentFile.IndexOf('\\'));
            string nukeDoubleBackslash = pastBackslashes.Replace('\\', '/');
            string nukeSpaces = nukeDoubleBackslash.Replace(' ', '_');

            return new Uri(RemoveAccents(nukeSpaces), UriKind.Relative);
        }

        private static string RemoveAccents(string input)
        {
            string normalized = input.Normalize(NormalizationForm.FormKD);
            Encoding removal = Encoding.GetEncoding(Encoding.ASCII.CodePage, new EncoderReplacementFallback(""), new DecoderReplacementFallback(""));
            byte[] bytes = removal.GetBytes(normalized);
            return Encoding.ASCII.GetString(bytes);
        }

    }

}
