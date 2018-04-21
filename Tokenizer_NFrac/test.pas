begin
    integer k;
    integer m;
    integer function ffffffffffffffffffffffffddd(n);
    begin
        integer n;
        if n <= 0 then F := 1
                  else F : n * F(n - 1)
    end;
    read(m);@
    k := F(m);
    write(k)
end