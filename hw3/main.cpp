#include <iostream>
#include <cstring>

using namespace std;

int words_naive(string text){
    int count = 0;
    if (text.size() == 0){
        return count;
    }
    if (text[0] == ' ') count--;
    for (int i = 0; i < text.length(); i++){
        if (i + 1 < text.length() && text[i] == ' ' && text[i + 1] != ' '){
            count++;
        }
    }
    return count + 1;
}

void compare(void const* first, void const* second, uint64_t* res){
    asm(R"(
        movups (%%rdi), %%xmm1
        pcmpeqb (%%rsi), %%xmm1
        movups %%xmm1, (%%rax)
    )"
    :
    :"S"(first), "D"(second), "a"(res)
    );
}


int words_advanced(char const * s) {
    size_t len = strlen(s);
    char const *spaces = "                ";
    int count = 0;
    if (len == 0) {
        return count;
    }
    int carry = 0;
    if (s[0] == ' ') count--;
    int i = 0;
    while (i + 16 < len) {
        uint64_t res[2];//результат compare, 2 по 8 байт
        compare(s + i, spaces, res);
        //cout << res[0] << " " << res[1] << " RES HERE "<< endl;
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 8; k++) {
                int cur = res[j] % 2;
                if (carry == 1 && cur == 0) {
                    count++;
                }
                carry = cur;
                res[j] >>= 8;//переход к следующему байту
            }
        }
        i += 16;
    }
    for (; i < len; i++) {
        if (carry == 1 && s[i] != ' ') {
            count++;
        }
        carry = (s[i] == ' ');
    }
    return count + 1;
}

void test1(){
    std::cout << "=================Running test 1=====================\n";
    char str[] = "Hello world!";
    cout << "The text is -> " << str << endl;
    cout << words_advanced(str) << " advanced anwer" << endl;
    cout << words_naive(str) << " naive answer" << endl;
    std::cout<< "==================Ending test 3=====================\n";
}

void test2(){
    std::cout << "=================Running test 2=====================\n";
    string str = "         some spacebars                                  lots                                  some   ";
    cout << "The text is -> " << str << endl;
    cout << words_advanced(str.c_str()) << " advanced anwer" << endl;
    cout << words_naive(str) << " naive answer" << endl;
    std::cout<< "==================Ending test 3=====================\n";
}

void test3() {
    std::cout << "=================Running test 3=====================\n";
    char str[] = "Английский защитник Джон Терри может в ближайшее время стать футболистом московского «Спартака». Как сообщает журналист Джанлука Ди Марцио на своём сайте, на сегодняшнее утро, 8 сентября, запланирован медицинский осмотр Терри в Риме. В случае удачного исхода обследования стороны могут заключить контракт.";
    cout << "The text is -> " << str << endl;
    std::cout << "The result is below\n";
    cout << words_advanced(str) << " advanced anwer" << endl;
    cout << words_naive(str) << " naive answer" << endl;
    std::cout<< "\n==================Ending test 3=====================\n";
}


void run_tests(){
    test1();
    test2();
    test3();
}

int main() {
    string text;
    getline(cin, text);
    const char* textChar = text.c_str();
    cout << words_advanced(textChar) << " advanced anwer" << endl;
    cout << words_naive(text) << " naive answer" << endl;
    //run_tests();
    return 0;
}