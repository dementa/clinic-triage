FLAGS=-std=c++17 -Wall -Wextra -g

.PHONY: all run test asan clean

all: clinic tests

clinic: main.cpp Patient.h PatientHeap.h Clinic.h
	g++ $(FLAGS) -o clinic main.cpp

tests: tests.cpp Patient.h PatientHeap.h Clinic.h
	g++ $(FLAGS) -o tests tests.cpp

run: clinic
	./clinic

test: tests
	./tests

# Same tests, with AddressSanitizer + UndefinedBehaviorSanitizer (finds memory errors).
asan:
	g++ $(FLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer -o tests_asan tests.cpp && ./tests_asan

clean:
	rm -f clinic tests tests_asan tmp_saved.txt tmp_bad.txt
