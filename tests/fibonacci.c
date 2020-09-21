int fib(n)
{
    if(n <= 1)
        return n;
    return fib(n -1) + fib(n - 2);
}

int main(void)
{
    int n[] = {1, 2, 3, 4};
    for(int i = 0; i < 4; i++)
        n[i] = fib(n[i]);
    return 0;
}
