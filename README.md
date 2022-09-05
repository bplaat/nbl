# NBL - New Bastiaan Language
This is a prototype interpreter for the NBL (New Bastiaan Language) programming language written in C11. It only uses the standard C library so it is very protable.

It is a mix of **JavaScript**, **PHP** and **Lua**. It is weird but also quite funny to program small programs in. The whole interpreter is around 3000 lines of code so not very big and it can easily be understood by one person. The interpreter is quite inefficient because it don't uses bytecodes it just traverses the AST to calculate all the values.

There is also a basic syntax highlighting extension for Visual Studio Code available. To install it you need to copy the `editors/vscode` folder into your `~/.vscode/extensions` folder.

## Things todo:
- Class inherintance, abstract classes and polymorpishm
- Byte code interpreter instead of ast traversing?
- Make vscode syntax highlighting better?

## Types:
- `null`
- `bool`
- `int`
- `float`
- `string`
- `array`
- `object`
- `function`
- `class`
- `instance`

## Comments
```
# Single line comment

// Single line comment

/*
Multi line comment
*/
```

## Variables
```
let x = 0;
x = 1;
const y = 2;
y = 3; // Error: can't mutate a const variable
```

## Arrays
```
const names = [ 'Bastiaan', 'Jan', 'Dirk' ];
println(names.length());
println(names[1]);

println([1,2,3,4,5].map(fn (x) => x * 2));
```

## Objects
```
fn Person(name, age) {
    return { name, age };
}

fn greet(person) => println('Name = ' + person.name + ', Age = ' + (string)person['age']);

const bastiaan = Person('Bastiaan', 20);
greet(bastiaan);
```

## Conditions
```
const number = 4.56;

if (number > 3) {
    println('Number is greater then 3!');
}
else if (number > 4) {

}
elseif (number > 4) {

}
elif (number > 4) {

}
else {

}
```

## Loops
```
// While loops
let x = 10;
while (x > 0) {
    x -= 1;
    continue;
}
println(x);

// Do while loops
x = 0;
do {
    x += 1;
    break;
} while(x > 10);
println(x);

// For loops
for (let i = 0; i < 100; i += 1) {
    println('-', i);
}

// For in loop
const names = [ 'Bastiaan', 'Dirk', 'Willem' ];
for (const name in names) {
    println(name);
}
```

## Functions
```
fn hello() {
    println('Hello World!');
}

const add = fn (a: int, b: int): int {
    return a + b;
};

fn double(number) => number * 2;

hello();
println(add(double(4), 4));
```

## Classes
```
class Person {
    SOME_STATIC_VAR = 'A constant',

    fn constructor(name, age) {
        this.name = name;
        this.age = age;
    }

    fn greet() {
        println('Name = ' + this.name + ', Age = ' + (string)this['age']);
    }
}

const bastiaan: instance = Person('Bastiaan', 20);
bastiaan.greet();
println(type(Person), Person);
println(type(bastiaan), bastiaan);
println(Person.SOME_STATIC_VAR, bastiaan.SOME_STATIC_VAR);
```
