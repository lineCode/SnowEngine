local CLionProjectMetaTable = {  __index = XcodeProjectMetaTable  }

function CLionProjectMetaTable:_WriteFilesFlat(folder)
    for entry in ivalues(folder) do
        if type(entry) == 'table' then
            self:_WriteFilesFlat(entry)
        else
            entry = entry:gsub("[() ]", "\\%1")
            self.Contents[#self.Contents + 1] = '    ' .. entry .. '\n'
        end
    end
end
--set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
--#add_executable(jamtest ${SOURCE_FILES})
--#add_custom_target(jamtest COMMAND s:/jamplus/bin/win64/jam -C ${jamtest_SOURCE_DIR} c.toolchain=win32/debug@C.COMPILER=mingw
                         --#CLION_EXE_DIR=${PROJECT_BINARY_DIR})
                         --
                         --
function CLionProjectMetaTable:Write(outputPath)
    local project = Projects[self.ProjectName]
    local filename = ospath.join(outputPath, self.ProjectName, 'CMakeLists.txt')

    self.Contents[#self.Contents + 1] = [[
cmake_minimum_required(VERSION 3.3)

project(]] .. self.ProjectName .. [[)

]]

    self.Contents[#self.Contents + 1] = [[
set(SOURCE_FILES
]]

    self:_WriteFilesFlat(project.SourcesTree)

    self.Contents[#self.Contents + 1] = [[
)

]]

    local platformName = 'win32'
    local configName = 'debug'

    local jamCommandLine = ospath.make_slash(jamScript) -- .. ' ' ..
            --'C.TOOLCHAIN=' .. platformName .. '/' .. configName
    self.Contents[#self.Contents + 1] = [[
add_custom_target(]] .. self.ProjectName .. [[ COMMAND "]] .. jamCommandLine .. [["
                        C.TOOLCHAIN=]] .. platformName .. [[/]] .. configName .. [[@C.COMPILER=mingw
                        CLION_EXE_DIR=${PROJECT_BINARY_DIR}
                        SOURCES ${SOURCE_FILES})
]]
--[=[
    if project.IncludePaths then
        --local includePaths = project.IncludePaths[Config.Platforms[1]]["debug"]
        local includePaths = project.IncludePaths[platformName][configName]
        self.Contents[#self.Contents + 1] = "target_include_directories(" .. self.ProjectName .. "\n"
        for _, includePath in ipairs(includePaths) do
            self.Contents[#self.Contents + 1] = '        "' .. includePath:gsub('\\', '/') .. '"\n'
        end
        self.Contents[#self.Contents + 1] = ')\n'
    end
    --]=]


    if project.IncludePaths then
        --local includePaths = project.IncludePaths[Config.Platforms[1]]["debug"]
        local includePaths = project.IncludePaths[platformName][configName]
        self.Contents[#self.Contents + 1] = "set_target_properties(" .. self.ProjectName .. " PROPERTIES INCLUDE_DIRECTORIES \"" .. table.concat(includePaths, ';'):gsub('\\', '/') ..  "\")\n"
    end

    if project.Defines then
        local defines = project.Defines[platformName][configName]
        self.Contents[#self.Contents + 1] = "set_target_properties(" .. self.ProjectName .. " PROPERTIES COMPILE_DEFINITIONS \"" .. table.concat(defines, ';') ..  "\")\n"
    end

    self.Contents = table.concat(self.Contents):gsub('\r\n', '\n'):gsub('\n', '\r\n')
    WriteFileIfModified(filename, self.Contents)

end

function CLionProject(projectName, options)
    return setmetatable(
        {
            Contents = {},
            ProjectName = projectName,
            Options = options,
        }, { __index = CLionProjectMetaTable }
    )
end


local CLionWorkspaceMetaTable = {  __index = CLionWorkspaceMetaTable  }


function CLionWorkspaceMetaTable:Write(outputPath)
    local workspace = Workspaces[self.Name]
end


function CLionWorkspace(workspaceName, options)
    return setmetatable(
        {
            Contents = {},
            Name = workspaceName,
            Options = options,
        }, { __index = CLionWorkspaceMetaTable }
    )
end



function CLionInitialize()
    local chunk = loadfile(ospath.join(_getTargetInfoPath(), 'ProjectExportInfo.lua'))
    if chunk then chunk() end
    if not ProjectExportInfo then
        ProjectExportInfo = {}
    end
end


function CLionShutdown()
    prettydump.dumpascii(ospath.join(_getTargetInfoPath(), 'ProjectExportInfo.lua'), 'ProjectExportInfo', ProjectExportInfo)
end




