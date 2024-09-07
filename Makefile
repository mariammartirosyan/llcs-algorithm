all: serial parallel-tasks parallel-taskloop

serial:
	${CC} llcs_main.c -o llcs-serial -O2

parallel-tasks: 
	${CC} llcs_main.c -o llcs-parallel-tasks -O2 -fopenmp -D _TASKS

parallel-taskloop: 
	${CC} llcs_main.c -o llcs-parallel-taskloop -O2 -fopenmp -D _TASKLOOP

clean:
	rm llcs-serial
	rm llcs-parallel-tasks
	rm llcs-parallel-taskloop
