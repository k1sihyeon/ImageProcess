diff_max=0.00001
fname=hw4

function missed_arg() {
    $1 $2 $3 $4 $5 $6 2> temp.txt
    if [ $? -eq 0 ] ; then
	echo "Fail: Program should use 4 args"
	exit 1
    fi
    output=$(cat temp.txt)    
    if [ -z "${output}" ] ; then
	echo "Fail: No error message for incorrent arguments"
	exit 1
    fi        
    if [[ "${output}" != *"Usage"* ]] && [[ "${output}" != *"${fname}"* ]] && [[ "${output}" != *"infile"* ]] ; then
	echo "Fail: Incorrect error message(${output})"
	exit 1
    fi
}

function invalid_arg() {
    $1 $2 $3 $4 $5 2> temp.txt
    if [ $? -eq 0 ] ; then
	echo "Fail: 4th parameter should be positive integer"
	exit 1
    fi
    output=$(cat temp.txt)    
    if [ -z "${output}" ] ; then
	echo "Fail: No error message for incorrent 4th arguments"
	exit 1
    fi        
    if [[ "${output}" != *"number"* ]] && [[ "${output}" != *"process"* ]] && [[ "${output}" != *"greater"* ]] ; then
	echo "Fail: Incorrect error message(${output})"
	exit 1
    fi
}

function missed_arg_test() {
    missed_arg "./${fname}"
    
    missed_arg "./${fname} data1.bin"

    missed_arg "./${fname} data1.bin data2.bin"

    missed_arg "./${fname} data1.bin data2.bin res.bin"

    missed_arg "./${fname} data1.bin data2.bin res.bin 3 4"

    invalid_arg "./${fname} data1.bin data2.bin res.bin 0"

    invalid_arg "./${fname} data1.bin data2.bin res.bin -2"
}

function file_size_t() {
    $1 $2 $3 $4 $5 2> temp.txt
    if [ $? -eq 0 ] ; then
	echo "Fail: Program cannot detect different file sizes"
	exit 1
    fi
    output=$(cat temp.txt)
    if [ -z "${output}" ] ; then
	echo "Fail: No error message for incorrent arguments"
	exit 1
    fi        
    if [[ "${output}" != *"file"* ]] && [[ "${output}" != *"size"* ]] && [[ "${output}" != *"not"* ]] && [[ "${output}" != *"same"* ]] ; then
	echo "Fail: Incorrect error message(${output}) for different file size."
	exit 1
    fi
}

function file_size_test() {
    file_size_t "./${fname} data1.bin id3.bin res.bin 4"

    file_size_t "./${fname} rand31.bin rand100.bin res.bin 4"
}

function basic_1x1_test(){ 
    output=$(./${fname} data1.bin data2.bin res.bin 1)
    if [ $? -eq 0 ] ; then
	echo "Pass: (1*1) Program exited zero"
    else
	echo "Fail: (1*1) Program did not exit zero"
	exit 1
    fi

    outputd=$(./hwdiffd res.bin data3.bin ${diff_max})
    if [ $? -eq 0 ] ; then
	echo "Pass: (1*1) Output is correct"
    else
	echo "Fail: (1*1) Output is not expected result"
	exit 1
    fi
}

function nxn_test(){
    output=$(./${fname} rand$1.bin inv_rand$1.bin res.bin 4)
    if [ $? -eq 0 ] ; then
	echo "Pass: ($1*$1) Program exited zero"
    else
	echo "Fail: ($1*$1) Program did not exit zero"
	exit 1
    fi

    outputd=$(./hwdiffd res.bin id$1.bin ${diff_max})
    if [ $? -eq 0 ] ; then
	echo "Pass: ($1*$1) Output is correct"
    else
	echo "Fail: ($1*$1) Output is not expected result"
	exit 1
    fi
}

function idxn_test(){
    output=$(./${fname} rand$1.bin id$1.bin res.bin 4)
    if [ $? -eq 0 ] ; then
	echo "Pass: (id$1*rand$1) Program exited zero"
    else
	echo "Fail: (id$1*rand$1) Program did not exit zero"
	exit 1
    fi

    outputd=$(./hwdiffd res.bin rand$1.bin ${diff_max})
    if [ $? -eq 0 ] ; then
	echo "Pass: (id$1*rand$1) Output is correct"
    else
	echo "Fail: (id$1*rand$1) Output is not expected result"
	exit 1
    fi
}

function proc_n_test(){
    ./${fname} rand1000.bin inv_rand1000.bin res.bin $1 &
    sleep 1
    output=$(ps aux | grep "./${fname}" | wc -l)
    if [ ${output} -gt $1 ] ; then
	echo "Pass: More than $1 processes(${output}) are generated"
    else
	echo "Fail: Too small processes(${output}) are forked"
	exit 1
    fi
}

function thread_n_test(){
    ./${fname} rand1000.bin inv_rand1000.bin res.bin $1 &
    sleep 1
    output=$(ps aux | grep "./${fname}" | wc -l)
    if [ ${output} -gt $1 ] ; then
	echo "Fail: Multiple processes are used"
	exit 1
    fi
    output2=$(ps | grep ${fname})
    PID=$(echo ${output2} | head -n1 | awk '{print $1;}')
    NT=$(ps huH p ${PID} | wc -l)
    if [ ${NT} -gt $1 ] ; then
	echo "Pass: More than $1 threads(${NT}) run"
    else
	echo "Fail: Too small threads(${NT}) run"
	exit 1
    fi
}

function thread_n_test2(){
    output=$(./${fname} rand1000.bin inv_rand1000.bin res.bin 5)
    if [ $? -eq 0 ] ; then
	echo "Pass: (rand1000*inv_rand1000) Program exited zero"
    else
	echo "Fail: (rand1000*inv_rand1000) Program did not exit zero"
	exit 1
    fi

    nop=$(echo ${output} | awk '{print $1;}')
    nopeak=$(echo ${output} | awk '{print $2;}')
    if [ ${nop} -eq 5 ] ; then
	echo "Pass: The number of active threads is ${nop}"
    else
	echo "Fail: The number of active threads(${nop}) is not 5"
	exit 1
    fi
    if [ ${nopeak} -gt 10 ] ; then
	echo "Pass: Thread peaks appear multiple times"
    else
	echo "Fail: Thread peaks appear seldom"
	exit 1
    fi
}

echo "Running tests..."
echo

case $1 in
    "1") missed_arg_test;;
    "2") file_size_test;;
    "3") basic_1x1_test;;
    "4") nxn_test "31";;
    "5") nxn_test "1000";;
    "6") idxn_test "1000";;
    "7") thread_n_test "4";;
    "8") thread_n_test2;;
    *) echo "Invalid argument ($1) for the script"
       exit 1;;
esac

echo "Test $1 passed."
exit 0
