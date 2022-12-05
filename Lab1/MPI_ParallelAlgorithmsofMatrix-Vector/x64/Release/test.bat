set mpiExeFileName=MPI_ParallelAlgorithmsofMatrix-Vector.exe
set logFileName=lol.log

for %%n in (2,4,8) do (
   echo ---------- >> .\%logFileName%
   echo %%n proc    >> .\%logFileName%
   echo ---------- >> .\%logFileName%
   echo --------------------------------------------------- >> .\%logFileName%
   for %%x in (10,100,1000,2000,3000,4000,5000,6000,7000,8000,9000,10000) do (                                                        
      @REM echo Size: %%x  Number of processes:%%n >> .\%logFileName%
      @REM echo %%x | mpiexec -n %%n %mpiExeFileName% >> .\%logFileName%
      echo %%x | %mpiExeFileName% >> .\%logFileName%
   )
)