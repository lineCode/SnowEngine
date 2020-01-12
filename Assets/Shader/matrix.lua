-- all monitor table
getvalue_metatable={
    -- key must be "val"
    __index=function(table,key)
        if(key=="val") then
            return GetVal(table);
        else
            return nil;
        end
    end
};

matrix_metatable={
    __index=function(table,key)
        return GetMatrix(key);
    end
};

matrix={};

vs_in_metatable={
    __index=function(table,key)
        return GetVsIn(key);
    end
};

vs_out_metable={
    __newindex=function(table,key,val)
        SetVsOut(key,val);
    end
};

ps_in_metatable={
    __index=function(table,key)
        return GetPsIn(key);
    end
};

vs_in={};
vs_out={};
ps_in={};

setmetatable(matrix,matrix_metatable);
setmetatable(vs_in,vs_in_metatable);
setmetatable(vs_out,vs_out_metable);
setmetatable(ps_in,ps_in_metatable);