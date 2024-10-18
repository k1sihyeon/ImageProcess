[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-8d59dc4de5201274e310e4c54b9627a8934c3b88527886e3b421487c677d23eb.svg)](https://classroom.github.com/a/p48ioyCV)
[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-c66648af7eb3fe8bc4f294546bfd86ef473780cde1dea487d3c4ff354943c9ae.svg)](https://classroom.github.com/online_ide?assignment_repo_id=9697793&assignment_repo_type=AssignmentRepo)
# 여러 프로세스로 실행되는 행렬곱 프로그램
This project is written in C, and tested with make and bash scripts.

### The assignment
- 두 이진 파일에서 각각 행렬 A와 행렬 B를 읽어 행렬곱 C를 구한다. 행렬 C를 이진 파일에 저장하는 C 프로그램을 구현한다. 이 프로그램이 읽는 이진 파일은 n\*n double 크기이며, 쓰는 이진 파일도 n\*n double 크기이다. 여기서 n은 1이상의 양수이므로 행렬곱을 수행하는 이 프로그램은 가능한 n\*n 크기의 행렬들을 모두 처리할 수 있어야 한다. 이 프로그램은 4개의 인자를 받는데, 2개의 인자는 입력 이진 파일 이름이고, 세 번째 인자는 출력 이진 파일 이름이다. 세 번째 인자의 이름과 같은 파일이 이미 있으면 덮어쓰기를 한다. 마지막 네 번째 인자는 이 프로그램이 실행하면서 생성할 자식 프로세스의 개수이며 1 이상의 정수라야 한다. 아래 예시에서 data1.bin은 행렬 A의 내용을 담고 있고, data2.bin은 행렬 B의 내용을 담고 있다. res.bin은 행렬곱 결과인 행렬 C의 내용을 담고 있으며 사용하는 프로세스는 1개이다. 정상적인 실행의 예시는 다음과 같다. 

`./hw2 data1.bin data2.bin res.bin 1`

- 정상적인 실행일 때 프로그램은 별도의 메시지를 출력하지 않고 세 번째 인자에 해당하는 파일만 생성하지만, 에러가 발생하는 경우 다음의 동작을 수행한다.
- hw2가 4개의 인자를 받지 못하면 다음 에러 메시지를 표준에러로 출력하고 종료한다.

`Usage: ./hw2 infile1 infile2 outfile <p>`

- 첫 번째 인자 파일의 크기가 n\*n double이 아니면 다음의 에러 메시지를 표준에러로 출력하고 종료한다.

`Error: The 1st input file size is not n*n`

- 두 번째 인자 파일의 크기가 첫 번째 인자 파일의 크기와 같지 않으면 다음의 에러 메시지를 표준에러로 출력하고 종료한다.

`Error: The 2nd input file size is not the same as the 1st one`

- 네 번째 인자의 값이 0 이하이면 다음의 에러 메시지를 표준에러로 출력하고 종료한다.

`Error: The 4th parameter should be an integer greater than zero`

- 작업 프로세스 개수인 네 번째 인자의 값이 행렬의 한쪽 면의 크기인 n보다 크면 작업 프로세스 개수를 n으로 줄인다.

### Setup command
N/A

### Run command to test program
`make test ARG=1`

`make test ARG=2`

...

`make test ARG=8`

### Notes
- `gcc` can be used to compile and link C applications for use with existing test harnesses or C testing frameworks.
