#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cstring>


void memcpy1(void* to, void* from)
{
    asm(R"(
        movb (%%rbx), %%ch
        movb  %%ch, (%%rax)
        )"
    //данные по адресу в rbx сохраняем в ch (ch - high CX - старшая половина CX)
    //переносим данные из ch по адресу в rax
    :
    :"b"(from), "a"(to) //rbx = from, rax = to
    :
    );
}

void memcpy1(void* to, void* from, size_t amount)
{
    for (int i = 0; i < amount; i++){
        memcpy1((__int8_t*) to + i, (__int8_t*) from + i);
    }
}

void memcpy8(void* to, void* from)
{
    asm(R"(
        movq (%%rbx), %%rcx
        movq %%rcx, (%%rax)
        )"
    //данные по адресу в rbx сохраняем в ecx (ecx - extended CX на 32 бита)
    //переносим данные из ecx по адресу в rax
    :
    :"b"(from), "a"(to) //rbx = from, rax = to
    :
    );
}

void memcpy8(void* to, void* from, size_t amount){
    size_t leftover = amount % 8;
    amount -= leftover;
    for (int i = 0; i < amount / 8; i++){
        memcpy8((__int8_t*)to + 8*i, (__int8_t*) from + 8*i);
    }
    memcpy1((__int8_t*)to + amount, (__int8_t*) from + amount, leftover);
}

void memcpy16_unaligned(void* to, void* from){
    asm(R"(
        movups (%%rbx), %%xmm0
        movups %%xmm0, (%%rax)
    )"
    //данные по адресу в rbx сохраняем в rcx
    //переносим данные из rcx по адресу в rax
    :
    :"b"(from), "a"(to) //rbx = from, rax = to
    :
    );
}

void memcpy16_unaligned(void* to, void* from, size_t amount){
    size_t leftover = amount % 16;
    amount -= leftover;
    for (int i = 0; i < amount / 16; i++){
        memcpy16_unaligned((__int8_t*)to + 8*i, (__int8_t*) from + 8*i);
    }
    memcpy8((__int8_t*)to + amount, (__int8_t*) from + amount, leftover);
}

void memcpy16_aligned(void* to, void* from){
    asm(R"(
        movaps (%%rbx), %%xmm0
        movaps %%xmm0, (%%rax)
        )"
    // xmm0 - регистры на 128 байт, добавлены в SSE, movaps соответственно mov на 16 байт
    //данные по адресу в rbx сохраняем в rcx
    //переносим данные из rcx по адресу в rax
    :
    :"b"(from), "a"(to) //rbx = from, rax = to
    :
    );
}

void memcpy16_aligned(void* to, void* from, size_t amount){
    while((((unsigned long long)to & 15) != 0 || ((unsigned long long)from & 15) != 0) && amount)
    {
        memcpy1(to, from, 1);
        to++;
        from++;
        amount--;
    }
    //memcpy16_unaligned(to, from, amount);
    size_t leftover = amount % 16;
    amount -= leftover;
    for (int i = 0; i < amount / 16; i++){
        memcpy16_aligned((__int8_t*)to + 8*i, (__int8_t*) from + 8*i);
    }
    memcpy8((__int8_t*)to + amount, (__int8_t*) from + amount, leftover);

}

void test1(){
    std::cout << "=================Running test 1=====================\n";
    char str[] = "Hello world!";
    std::cout << "Calling 1 byte memcpy to copy string -> " << str <<  std::endl;
    void* copy_destination = malloc(sizeof(str));
    memcpy1(copy_destination, str, (strlen(str)) * sizeof(char));
    printf("%s\n", static_cast<char *>(copy_destination));
    std::cout<< "==================Ending test 3=====================\n";
}

void test2(){
    std::cout << "=================Running test 2=====================\n";
    long nums[] = {0xfffabcfcaf, 0xeeeeeeee, 0xabcdefff, 0xffffffffff, 1};
    long copy_destination[] = {0, 0, 0, 0, 0};
    std::cout << "Calling 8 byte memcpy to copy array -> ";
    for (auto i: nums){
        std::cout << i << ' ';
    }
    std::cout << std::endl;
    memcpy8(copy_destination, nums, sizeof(nums));
    std::cout << "The result is below\n";
    for (auto i: copy_destination){
        std::cout << i << ' ';
    }
    std::cout<< "\n==================Ending test 3=====================\n";
}

void test3() {
    std::cout << "=================Running test 3=====================\n";
    char str[] = "Английский защитник Джон Терри может в ближайшее время стать футболистом московского «Спартака». Как сообщает журналист Джанлука Ди Марцио на своём сайте, на сегодняшнее утро, 8 сентября, запланирован медицинский осмотр Терри в Риме. В случае удачного исхода обследования стороны могут заключить контракт.";
    std::cout << "Calling 16 byte memcpy to copy the text\n";
    void* copy_destination = malloc(sizeof(str));
    memcpy16_unaligned(copy_destination, str, (strlen(str)) * sizeof(int));
    std::cout << "The result is below\n";
    std::cout << static_cast<char *>(copy_destination);
    std::cout<< "\n==================Ending test 3=====================\n";
}


void run_tests(){
    test1();
    test2();
    test3();
}

int main() {
    char str[] = "thisispossiblyrecognizabletext";
    std::cout << str << " length is 30 btw\n";
    void* copy_destination = malloc(sizeof(str));
    memcpy16_aligned(copy_destination, str, (strlen(str)) * sizeof(char));
    printf("%s\n", static_cast<char *>(copy_destination));
    run_tests();
    return 0;
}