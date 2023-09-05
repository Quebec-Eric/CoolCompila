(*
    Aluno: Bruno Pena Baêta
    Matrícula: 696997
    Professor: Matheus Alcântara
    Matéria: Compiladores
*)

class Utils inherits IO {
    or(b1 : Bool, b2 : Bool) : Bool {
    {
        if (b1) then true else
        {
            if (b2) then true else false fi;
        } fi;
    }
    };

    and(b1 : Bool, b2 : Bool) : Bool {
    {
        if (b1) then {
            if (b2) then true else false fi;
        } else false fi;
    }
    };
};

CLASS Stack inherits Utils {
    value : String <- "";
    stack : Stack;

    getValue() : String {
        {value;}
    };

    setValue(v : String) : Int {
    {
        value <- v;
        0;
    }
    };

    getStack() : Stack {
        {stack;}
    };

    setStack(s : Stack) : Int {
    {
        if (isvoid s) then 0 else
        {
            stack <- s;
            0;
        } fi;
    }
    };
};

class StackManager inherits Stack {
    vs1 : String;
    vs2 : String;
    vi1 : Int;
    vi2 : Int;
    converter : A2I <- new A2I;

    print(s : Stack) : Int {
    {
        if (s.getValue() = "") then 0 else 
        {
            out_string(s.getValue());
            out_string("\n");
            if (isvoid s.getStack()) then 0 else print(s.getStack()) fi;
        } fi;
    }
    };

    execute(s : Stack) : StackManager {
    {
        if (or(s.getValue() = "+", s.getValue() = "s")) then {
            vs1 <- s.getStack().getValue();
            vs2 <- s.getStack().getStack().getValue();
            self.setStack(s.getStack().getStack().getStack().copy());
            if (s.getValue() = "+") then {
                vi1 <- converter.a2i(vs1);
                vi2 <- converter.a2i(vs2);
                self.setValue(converter.i2a(vi1 + vi2));
            } else {
                self.setValue(vs1);
                self.setStack(self.copy());
                self.setValue(vs2);
            } fi;
            self;
        } else {
            self;
        } fi;
    }
    };
};

class Main inherits Utils{
    stack : StackManager;
    e : String;

    do() : Int {
    {
        out_string("> ");
        e <- in_string();
        if e = "x" then 0 else 
        {
            if e = "d" then stack.print(stack) else
            {
                if e = "e" then stack <- stack.execute(stack) else
                {
                    stack.setStack(stack.copy());
                    stack.setValue(e);
                } fi;
            } fi;
            do();
        } fi;
    }
    };

    main() : Int {
    {
        out_string("START:\n");
        stack <- new StackManager;
        do();
        0;
    }
    };
};