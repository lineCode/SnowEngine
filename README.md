# SnowEngine

## A Simple GameEngine Based On GCC4

### Dirctories:
- /Assets:
    Resources which consisits of lua scripts, xml config, png shader sources,etc

- /Game
    Just a zip of assets

- /Source
    + /Engine              Source Of Engine<br />
    + /Editor              Source of Editor DLL And GameEditor<br />
    + /TeapotWars          A Game Based on GCC4 Engine <br />
    + /Lib                 The output of lib and exe 

### How To Intsall:
1. Download [DirectX Package](https://www.microsoft.com/en-us/download/details.aspx?id=6812) and intsall (in support of DirectX 11 )
2. Install Visual Stdio 2019
3. Open /Source/TeapotWars/Msvc/TeapotWars.sln and Run

### Notice:
1. If you want get release version you should complier all 3rdParty Lib
2. Some Comments in Code is written by Chinese 

### Code Architect
- Engine->GameBase(GameLogic)->GameView
- Render And GamePlay are separated

### Features Will be Added:
1. UI will be configured By Lua Script
2. A more powerful Rendering System
3. Use Lua To extend Material
4. Keyboard and Mouse Controller Reconstuction
...

### What is Developing
Since Now,I am develeoping the shader system powered by Lua.
You can write shader use lua grammar. This is an example:
```lua
function config()
    return {
        lightmodel="custom",
        custom=custom_config;
    };
end

--if we don't use custom configuration...
--[[
function config()
    return {
        lightmodel="phong",
        material={
            ambient="/texture/ambient.png",
            diffuse="/texture/diffuse.png",
            specular="/texture/specular.png",
            normal_map="/texture/normal.png"
        };
    };
end 
]]

function custom_config()
    set_render_quene(RenderQueue.Opaque);
    create_samplestate("samplestate1",Filter.MIN_LINEAR_MAG_MIP_POINT,WRAP,WRAP);
    add_texture("diffuseMap");
    add_float4("albedo");
    use_depth_test(true);
    --use_ao or use_shadow will create an ao/shadow map automatically
    use_ao(false);
    use_shadow(true);
    set_vs_shader(vs);
    set_ps_shader(ps);
    -- you can also write like this...
    return {
        render_queue=Opaque,
        ao=false,
        depth_test=true,
        shadow=false,
        vs_shader=vs,
        ps_shader=ps
    }
end

-- sv_position is the var will be noramlized by divided w
-- so you must assign sv_position,other params in vs_out
-- will be added into ps's ps_in variable automatically...

function vs()
    local pos=vs_in.position;
    local posw=mul(float4(pos,1.0),matrix.world);
    local posh=mul(posw,matrix.viewproj);
    local buff={};
    buff[0] = posw;
    buff[1] = posh;
    vs_out.sv_position=buff[0];
    --you can define anything into vs_out 
    vs_out.world_position=pos;
    vs_out.normal=vs_in.normal;
    vs_out.texcord=vs_in.texcord;
end

-- in PS you must return color and we will check if you return float4 variable
-- some functions may not exist in HLSL and Compiler can convert into HLSL or GLSL

function ps()
    local posh=ps_in.sv_position;
    local posw=ps_in.world_position;
    local uv=ps_in.texcord;
    local diffuseColor=sample("diffuseMap",uv,"samplestate1");
    local res=mul(diffuseColor,albedo);
    return normalize(diffuseColor);
end
```