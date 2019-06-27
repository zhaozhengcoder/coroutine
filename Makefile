all : main demo1

main : main.c coroutine.c
	gcc -g -Wall -o $@ $^

coroutine.o:coroutine.c 
	gcc -c coroutine.c

# 这个有点傻逼了，demo是用c++写的，编译的时候，发现协程库是c写的
# 构建的时候稍稍麻烦一点，c++ 连接c的代码
demo1 : test_demo1.c coroutine.o
	g++ -g -o $@ $^

clean :
	rm main demo1
