-- Teste: criação de classes
class A inherits IO {
    a1 : Int; -- Teste: atributo não inicializado
    a2 : Int <- 100; -- Teste: atributo inicializado

    f() : Int { 1 }; -- Teste: definição de métodos
};

class B inherits A {
    b1 : String; 
    b2 : String <- "abc";

    g() : Int { 2 }; 
};

class C inherits A {
    c1 : Bool; 
    c2 : Bool <- true;

    f() : Int { 3 }; 
    h() : Int { 4 }; 
};

class D inherits B {};

class E inherits B {};

class F inherits C {
  getA2() : Int { a2 };
};

class Main inherits E {
  x : SELF_TYPE;

  getB2() : String { b2 }; 

  out_string(str : String) : SELF_TYPE {
    self@IO.out_string(str.concat("\n"))
  };

  out_int(n : Int) : SELF_TYPE {
    self@IO.out_int(n)@IO.out_string("\n")
  };

  -- Teste: new SELF_TYPE
  i() : Object {
      {
          x <- new SELF_TYPE;
          out_string(x.type_name());
      }
  };

  -- Teste: case
  j(x : Object) : Object {
      case x of
          i : Int => out_int(1 + i);
          s : String => out_string("Encontrou ".concat(s));
          a : A => out_int(a.f());
          m : Main => out_int(m.g());
      esac
  };

  main(): Object {
    {
        -- self.abort()
        -- abort()
        -- x.abort()

        new G.i();

        -- Teste: despacho IO
        out_string("abc");
        out_int(100);
        out_string("abc").out_int(100);

        -- Teste: despacho objeto
        out_string(type_name());
        out_string(100.type_name());
        out_string("abc".type_name());
        out_string(true.type_name());
        out_int(100.copy());

        -- Teste: despacho String
        out_int("abcde".length());
        out_string("abc".concat("de"));
        out_string("abcde".substr(3, 2));

        -- Teste: despacho atributo não inicializado
        out_int(a1);
        out_string(b1);

        -- Teste: despacho e atribuição de atributo inicializado
        out_int(a2);
        a2 <- 150;
        out_int(a2);

        out_string(b2);
        b2 <- "def";
        out_string(b2);

        --Teste: let com e sem inicialização de atributos
        let r : Int, 
            s : Int <- 100, 
            t : String, 
            u : String <- "abc",
            f : F <- new F,
            a : A <- new A, 
            b : A <- new B, 
            c : A <- new C,
            x1 : Int <- 1, 
            y1 : Int <- 2,
        -- Teste: while
            x2 : Int, 
            y2 : Int,
            z : Object <-
            while x2 <= 10 loop
                {
                y2 <- y2 + x2;
                x2 <- x2 + 1;                
                }
            pool
        in {
            out_int(y2);
            out_int(x2);
            out_string(if isvoid z then "OK" else "falhou" fi);

            out_int(r);
            r <- 150;
            out_int(r);
            out_int(s);
            s <- 200;
            out_int(s);

            out_string(t);
            t <- "def";
            out_string(t);
            out_string(u);
            u <- "ghi";
            out_string(u);

            -- Teste: nome de nova classe
            out_string(f.type_name());
            out_int(f.getA2());

            -- Teste: despacho dinâmico
            out_int(a.f());
            out_int(b.f());
            out_int(c@A.f()); -- Teste: despacho estático

            -- Teste: operações aritméticas
            out_int(x1 + y1);
            out_int(x1 - y1);
            out_int(x1 * y1);
            out_int(x1 / y1);
            out_int(~x1);
            out_int(~3 * (4 - 12 / 6));
        };

        -- Teste: if
        out_string(if true then "YES1" else "NO1" fi);
        out_string(if false then "YES2" else "NO2" fi);

        -- Teste: isvoid
        out_string(if isvoid x then "x void" else "x not void" fi);
        out_string(if isvoid self then "self void" else "self not void" fi);

        -- Teste: not
        out_string(if not true then "NO1" else "YES1" fi);
        out_string(if not false then "NO2" else "YES2" fi);

        -- Teste: igualdade e comparações
        out_string(if 3 = 3 then "3 OK" else "3 falhou" fi);
        out_string(if self = self then "self OK" else "self falhou" fi);
        out_string(if x = self then "comp OK" else "comp falhou" fi);
        out_string(if 3 < 5 then "OK" else "falhou" fi);
        out_string(if 3 <= 5 then "OK" else "falhou" fi);
        out_string(if 5 < 3 then "OK" else "falhou" fi);
        out_string(if 5 <= 3 then "OK" else "falhou" fi);
        out_string(if 3 < 3 then "OK" else "falhou" fi);
        out_string(if 3 <= 3 then "OK" else "falhou" fi);

        -- Teste: case
        j(100);
        j("abc");
        j(self);
        j(new A);
    }
  };
};

class G inherits Main {};