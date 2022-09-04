# NBL - New Bastiaan Language
This is a prototype interpreter for the NBL (New Bastiaan Language) programming language written in C11. It only uses the standard C library so it is very protable.

It is a mix of **JavaScript**, **PHP** and **Lua**. It is weird but also quite funny to program small programs in. The whole interpreter is around 3000 lines of code so not very big and it can easily be understood by one person. The interpreter is quite inefficient because it don't uses bytecodes it just traverses the AST to calculate all the values.

## Things todo:
- Make stdlib better?
- Classes?
- Byte code interpreter?
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
println(array_length(names)); // -> 3
println(names[1]); // -> 'Jan'
```

## Objects
```
const person = {
    name = 'Bastiaan',
    age = 17
};

println('Name = ' + person.name + ', Age = ' + (string)person['age'])
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
println(add(double(4), 4)); // -> 12
```
