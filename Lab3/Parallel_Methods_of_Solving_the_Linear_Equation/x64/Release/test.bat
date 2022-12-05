set mpiExeFileName=Parallel_Methods_of_Solving_the_Linear_Equation.exe
set logFileName=lol.log

for %%n in (2,4,8) do (
   echo ---------- >> .\%logFileName%
   echo %%n proc    >> .\%logFileName%
   echo ---------- >> .\%logFileName%
   echo --------------------------------------------------- >> .\%logFileName%
   for %%x in (10,100,500,1000,1500,2000,2500,3000) do (                                                        
      @REM echo Size: %%x  Number of processes:%%n >> .\%logFileName%
	  echo 'echo %%x | mpiexec -n %%n %mpiExeFileName% >> .\%logFileName%'
      echo %%x | mpiexec -n %%n %mpiExeFileName% >> .\%logFileName%
      @REM echo %%x | %mpiExeFileName% >> .\%logFileName%
   )
)