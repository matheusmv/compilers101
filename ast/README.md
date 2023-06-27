# ROSE (run once segfault everywhere)

1. Compilar interpretador:

```shell
make rose
```

2. Executando um programa na linguagem:

```shell
./rose <programa>.rose
```

# Tipos de Dados

A linguagem suporta os seguintes tipos de dados:

 - `int`: representa números inteiros.
 - `float`: representa números de ponto flutuante.
 - `bool`: representa valores booleanos (true ou false).
 - `string`: representa sequências de caracteres.
 - `char`: representa um caractere.

# Operadores Aritméticos

A linguagem suporta os seguintes operadores aritméticos:

 - `+`: adição.
 - `-`: subtração.
 - `*`: multiplicação.
 - `/`: divisão.
 - `%`: resto da divisão inteira.

# Operadores Lógicos e Bitwise

A linguagem suporta os seguintes operadores lógicos e bitwise:

 - `~`: negação bitwise.
 - `&`: operação AND bitwise.
 - `|`: operação OR bitwise.
 - `^`: operação XOR bitwise.
 - `<<`: deslocamento à esquerda bitwise.
 - `>>`: deslocamento à direita bitwise.

# Operadores Relacionais

A linguagem suporta os seguintes operadores relacionais:

 - `==`: igual a.
 - `!=`: diferente de.
 - `<=`: menor ou igual a.
 - `>=`: maior ou igual a.
 - `<`: menor que.
 - `>`: maior que.

# Operadores Lógicos

A linguagem suporta os seguintes operadores lógicos:

 - `&&`: operador AND lógico.
 - `||`: operador OR lógico.
 - `!`: operador NOT lógico.

# Operadores de Concatenação

A linguagem suporta o operador + para concatenar strings.

# Variáveis

Variáveis podem ser declaradas usando a palavra-chave `let` seguida pelo nome da variável e, opcionalmente, pelo tipo e valor inicial.

Exemplo:

```js
let a = 10;
let b: float = 3.14;
let c: string = "hello";
```

# **Operações de Atribuição de Variáveis**

A linguagem suporta as seguintes operações de atribuição:

 - `=`: atribui o valor à variável.
 - `+=`: adiciona um valor à variável e atribui o resultado.
 - `-=`: subtrai um valor da variável e atribui o resultado.
 - `*=`: multiplica a variável por um valor e atribui o resultado.
 - `/=`: divide a variável por um valor e atribui o resultado.
 - `%=`: calcula o resto da divisão da variável por um valor e atribui o resultado.
 - `&=`: realiza uma operação AND bitwise entre a variável e um valor e atribui o resultado.
 - `|=`: realiza uma operação OR bitwise entre a variável e um valor e atribui o resultado.
 - `^=`: realiza uma operação XOR bitwise entre a variável e um valor e atribui o resultado.
 - `<<=`: realiza um deslocamento à esquerda bitwise na variável e atribui o resultado.
 - `>>=`: realiza um deslocamento à direita bitwise na variável e atribui o resultado.

Exemplo:

```js
let a = 5;
a += 2; // a = a + 2
println(a); // Saída: 7

let b = 10;
b -= 3; // b = b - 3
println(b); // Saída: 7

let c = 3;
c *= 4; // c = c * 4
println(c); // Saída: 12

let d = 20;
d /= 5; // d = d / 5
println(d); // Saída: 4

let e = 15;
e %= 4; // e = e % 4
println(e); // Saída: 3

let f = 7;
f &= 3; // f = f & 3
println(f); // Saída: 3

let g = 10;
g |= 3; // g = g | 3
println(g); // Saída: 11

let h = 8;
h ^= 2; // h = h ^ 2
println(h); // Saída: 10

let i = 6;
i <<= 2; // i = i << 2
println(i); // Saída: 24

let j = 16;
j >>= 2; // j = j >> 2
println(j); // Saída: 4
```

# Estruturas de Controle

1. **Loops while**

A estrutura de controle `while` é usada para repetir um bloco de código enquanto uma condição é verdadeira.

Exemplo:

```js
let i = 0;
while (i < 10) {
    println(i);
    i++;
}
```

2. **Loops for**

A estrutura de controle `for` é usada para iterar sobre uma sequência de valores.

Exemplo:

```js
for (let i = 0; i < 10; i++) {
    println(i);
}
```

3. **Declaração if, else if e else**

A estrutura de controle `if`, `else if` e `else` é usada para executar um bloco de código com base em uma condição.

Exemplo:

```js
let a = 10;
if (a > 5) {
    println("a é maior que 5");
} else if (a == 5) {
    println("a é igual a 5");
} else {
    println("a é menor que 5");
}
```

4. **Declaração continue**

A declaração `continue` é usada para interromper a iteração atual de um loop e iniciar a próxima iteração.

Exemplo:

```js
for (let i = 0; i < 10; i++) {
    if (i % 2 == 0) {
        continue;
    }
    println(i);
}
```

. **Declaração break**

A declaração `break` é usada para interromper a iteração atual de um loop.

Exemplo:

```js
for (let i = 0; i < 10; i++) {
    if (i == 5) {
        break;
    }
    println(i);
}
```

# **Funções**

Funções podem ser definidas usando a palavra-chave `func` seguida pelo nome da função, parâmetros entre parênteses e um tipo de retorno opcional.

Exemplo:

```js
func add(a: int, b: int): int {
    return a + b;
}
```

Funções também podem ser atribuídas a variáveis.

Exemplo:

```js
let addFunc = func(a: int, b: int): int {
    return a + b;
};
```

# Entrada e Saída

 - A função `print` é usada para imprimir uma linha de texto no console.
 - A função `println` é usada para imprimir uma linha (com quebra `\n`) de texto no console.
 - A função `input` é usada para receber entrada do usuário. Ela retorna uma string.

Exemplo:

```js
let username = input("Digite seu nome: ");
println("Olá, " + username + "!");
```

# **Utilitários**

 - A função `len` é usada para obter o tamanho de um objeto. Retorna o tamanho de uma string ou array.

# **Comentários**

Comentários podem ser inseridos usando `//` para comentários de uma linha ou `/* ... */` para comentários de várias linhas.

Exemplo:

```js
// Isto é um comentário de uma linha

/*
   Isto é um comentário de
   várias linhas.
*/

```
