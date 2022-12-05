set mpiExeFileName=MPI_ParallelAlgorithmsofMatrix-Matrix.exe
set logFileName=lol.log

for %%n in (4,9) do (
   echo ---------- >> .\%logFileName%
   echo %%n proc    >> .\%logFileName%
   echo ---------- >> .\%logFileName%
   echo --------------------------------------------------- >> .\%logFileName%
   for %%x in (10,100,500,1000,1500,2000,2500,3000) do (                                                        
      @REM echo Size: %%x  Number of processes:%%n >> .\%logFileName%
      echo %%x | mpiexec -n %%n %mpiExeFileName% >> .\%logFileName%
      @REM echo %%x | %mpiExeFileName% >> .\%logFileName%
   )
)