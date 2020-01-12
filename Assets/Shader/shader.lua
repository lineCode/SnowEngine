require "Shader\\matrix.lua"

-- overload operators
floatmetatable={
    __mul=floatmul,
    __add=floatadd,
    __sub=floatsub,
    __div=floatdiv,
    __index=function(table,key)
        if(key=="val") then
            return GetVal(table);
        else
            return nil;
        end
    end
};

float2metatable={
    __mul=float2mul,
    __add=float2add,
    __sub=float2sub,
    __div=float2div,
    __index=function(table,key)
        if(key=="val") then
            return GetVal(table);
        else
            return nil;
        end
    end
};

float3metatable={
    __mul=float3mul,
    __add=float3add,
    __sub=float3sub,
    __index=function(table,key)
        if(key=="val") then
            return GetVal(table);
        else
            return nil;
        end
    end
};

float4metatable={
    __mul=float4mul,
    __add=float4add,
    __sub=float4sub,
    __index=function(table,key)
        if(key=="val") then
            return GetVal(table);
        else
            return nil;
        end
    end
};

-- type defination
function float(x)
    return GetFloat(x);
end

function float2(x,y,z,w)
    return GetFloat2(x,y);
end

function float3(x,y,z,w)
    return GetFloat3(x,y,z);
end

function float4(x,y,z,w)
    return GetFloat4(x,y,z,w);
end

function float3x3(x,y,z)

end

function float4x4(x,y,z,w)

end

--shader functions
function mul(vec1,vec2)
    return GetMul(vec1,vec2);
end

function dot(vec1,vec2)
    return GetDot(vec1,vec2);
end

function normalize(vec)
    return Normalize(vec);
end

function sample(map,uv,samplestate)
    return Sample(map,uv,samplestate);
end

--config functions
function set_vs_shader(vsFunc)
    SetVsShader(vsFunc);
end

function set_ps_shader(psFunc)
    SetPsShader(psFunc);
end

function set_render_quene(queue)
    local isVaild=false;
    for k,v in ipairs(ShaderQueue) do
        if queue==v then
            isVaild=true;
        end
    end
    if(isVaild) then
        SetRenderQueue(queue);
    end
end

function create_samplestate(samplestatename,filter,umode,vmode)
    if(filter<34984553 or filter>34984556) then filter=34984553; end
    if(umode<47867821 or umode>47867824) then umode=47867821; end
    if(vmode<47867821 or vmode>47867824) then vmode=47867821; end
    CreateSampleState(samplestatename,filter,umode,vmode);
end

function add_texture(map_dir,map_name)
    AddTexture(map_dir,map_name);
end

function add_float(var_name,x)
    if(x==nil) then
        AddFloat(name);
    else
        AddFloatA(name,x);
    end
end

function add_float2(var_name,x,y)
    if(x==nil) then
        AddFloat2(name);
    else
        AddFloat2A(name,x,y);
    end
end

function add_float3(var_name,x,y,z)
    if(x==nil) then
        AddFloat3(name);
    else
        AddFloat3A(name,x,y,z);
    end
end

function add_float4(var_name,x,y,z,w)
    if(x==nil) then
        AddFloat4(name);
    else
        AddFloat4A(name,x,y,z,w);
    end
end

function use_depth_test(isOpen)

end

function use_ao(isOpen)
    
end

function use_shadow(isOpen)
    
end

--some Macros Numbers are random just for being rewriten by mistake
MIN_LINEAR_MAG_MIP_POINT=34984553;
MIN_MAG_MIP_LINEAR=34984554;
MIN_POINT_MAG_LINEAR_MIP_POINT=34984555;
ANISOTROPIC=34984556;

WRAP=47867821;
BORDER=47867822;
CLAMP=47867823;
MIRROR=47867824;

-- user can also use this for convenience
Filter={
    MIN_LINEAR_MAG_MIP_POINT=34984553,
    MIN_MAG_MIP_LINEAR=34984554,
    MIN_POINT_MAG_LINEAR_MIP_POINT=34984555,
    ANISOTROPIC=34984556,
};

Address={
    WRAP=47867821,
    BORDER=47867822,
    CLAMP=47867823,
    MIRROR=47867824
}

Background=1000;
Opaque=2000;
AlphaTest=2450;
Transparent=3000;
Overlay=4000;
GeometryLast=9000;

RenderQueue={
    Background=1000,
    Opaque=2000,
    AlphaTest=2450,
    Transparent=3000,
    Overlay=4000,
    GeometryLast=9000
};

