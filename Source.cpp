#include <iostream>
#include <vector>
#include <cstdlib>
#include <boost/random.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/integer.hpp>

using namespace boost::multiprecision;
using namespace boost::math;
using namespace std;

//оптимизация (сохранение значений p и q)
struct Param
{
    cpp_int p;
    cpp_int q;
};

struct Keys {
    cpp_int n;
    cpp_int e;
    cpp_int d;
};

cpp_int RandomBits(int len) {
    if (len < 1) return 0;
    cpp_int random = rand() % 2;
    for (int i = 0; i < len - 1; ++i) {
        random = random << rand() % 2;
    }
    return random;
}

cpp_int GetRandom(cpp_int from, cpp_int to) {
    int len = msb(to) + 1;
    cpp_int random = RandomBits(rand() % len + 1);
    while (!(random >= from && random <= to))
        random = RandomBits(rand() % len + 1);
    return random;
}

cpp_int GenE(cpp_int fi) {
    cpp_int e = GetRandom(2, fi - 1);;
    while (gcd(e, fi) != 1) {
        e = GetRandom(2, fi - 1);
    }
    return e;
}

//миллер - рабин
bool IsPrime(cpp_int n) {
    if (n % 2 == 0 || n <= 2) return false;

    int s = 0;
    cpp_int t;
    do {
        t = n / 2;
        s++;
    } while (n % 2 == 0);

    cpp_int x, a;

    for (int i = 0; i < 100; i++) {
        a = GetRandom(2, n - 2);
        if (gcd(a, n) != 1) return false;
        x = powm(a, t, n);
        if (x == 1 || x == n - 1)
            continue;

        for (int r = 1; r < s; r++) {
            x = powm(x, 2, n);
            if (x == 1) return false;
            if (x == n - 1)  break;
        }
        if (x != n - 1)  return false;
    }
    return true;
}

cpp_int GenPrime() {
    cpp_int res = RandomBits(1024);
    while (!IsPrime(res)) {
        res = RandomBits(1024);
    }
    return res;
}

cpp_int ExEu(cpp_int a, cpp_int b) {
    cpp_int temp = b;
    cpp_int div, res, m, n;
    cpp_int x = 1, y = 0;
    cpp_int x_1 = 0, y_1 = 1;
    while (a != 0) {
        div = b / a;
        res = b % a;
        m = x_1 - div * x;
        n = y_1 - div * y;
        x_1 = x, y_1 = y;
        x = m, y = n;
        b = a, a = res;
    }
    return (x_1 + temp) % temp;
}

Keys Gen(Param& param) {
    Keys keys;
    param.p = GenPrime();
    param.q = GenPrime();
    keys.n = param.p * param.q;
    cpp_int fi = (param.p - 1) * (param.q - 1);
    keys.e = GenE(fi);
    keys.d = ExEu(keys.e, fi);
    return keys;
}

cpp_int Encr(Keys key, cpp_int value) {
    return  powm(cpp_int(value), key.e, key.n);
}

cpp_int Decr(Keys key, cpp_int value) {
    return powm(cpp_int(value), key.d, key.n);
}

void MessageOperations(Keys keys, Param param, string str) {
    vector <cpp_int> encr, decr;
    for (int i = 0; i < str.length(); i++) {
        encr.push_back(Encr(keys, cpp_int(str[i])));
        decr.push_back(Decr(keys, encr[i]));
    }
    cout << endl << "Encrypted message: " << endl;
    for (int i = 0; i < str.length(); i++)
        cout << (char)encr[i];
    cout << endl << "Decrypted message: " << endl;
    for (int i = 0; i < str.length(); i++)
        cout << (char)decr[i];
}

int main() {
    srand(time(NULL));
    Param param;
    Keys keys = Gen(param);
    cout << "Key p: \n" << param.p << endl << endl;
    cout << "Key q: \n" << param.q << endl << endl;
    cout << "Value e: \n" << keys.e << endl << endl;
    cout << "Value d: \n" << keys.d << endl << endl;

    string test = "Hello, world!";
    MessageOperations(keys, param, test);
    return 0;
}


/*
cpp_int J(cpp_int a, cpp_int value) {
    if (value % a == 0) return 0;
    value = value % a;
    int t = 1;
    cpp_int r;
    while (value != 0) {
        while (value % 2 == 0) {
            value = value / 2;
            r = a % 8;
            if (r == 3 || r == 5)
                t = -t;
        }
        cpp_int temp = value;
        value = a;
        a = temp;
        if (value % 4 == 3 && a % 4 == 3)
            t = -t;
        value = value % a;
    }
    if (a == 1)
        return t;
    else
        return 0;
}

//соловей - штрассен
bool IsPrime_second(cpp_int value) {
    if (value % 2 == 0 || value < 2) return false;
    cpp_int a;
    for (int i = 0; i < 100; i++) {
        a = gen_random(2, value - 1);
        cout << "a = " << a << endl;
        if (gcd(a, value) > 1) return false;
        cpp_int check = powm(a, ((value - 1) / 2), value);
        cpp_int j = J(a, value);
        if (check != j && (check - value) != j && (value - check) != j) {
            //cout << "res : "<< powm(a, (value - 1) / 2, value) << endl;
            return false;
        }
    }
    return true;
}

*/