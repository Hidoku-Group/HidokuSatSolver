import sys
import subprocess
import string
import random
import os;
import shutil;
from subprocess import *
import resource

import signal

#
# set the default timeout to some value
#
DEFAULT_TIMEOUT = 20
DEFAULT_MEMORYLIMIT = 1024*1024*1024*4



#
# Set maximum CPU time to the global timeout
#
GLOBALTIMEOUT = DEFAULT_TIMEOUT
GLOBALMEMORYLIMIT = DEFAULT_MEMORYLIMIT

def setlimits():
    #print "Setting resource limit in child (pid %d)" % os.getpid()
		resource.setrlimit(resource.RLIMIT_CPU, (GLOBALTIMEOUT, GLOBALTIMEOUT+1))
		resource.setrlimit(resource.RLIMIT_AS,  (GLOBALMEMORYLIMIT, GLOBALMEMORYLIMIT+1))

#
# Check whether a string is a number
#

def is_number(s):
    try:
        int(s)
        return True
    except ValueError:
        return False


#
# Run the competition with the solvers and files specified in the list
#

def main( solversName, filesName, unsatInstancesList ): 
# print PID
	pid = os.getpgid(0)
	competitionDir = os.getcwd()
	print "process id: " + str(pid)
# read the two files
	solversFile = open(solversName, 'r')
	filesFile   = open(filesName,   'r')
	unsatFile   = open( unsatInstancesList, 'r' )
	
	solverData  = solversFile.readlines()
	fileData    = filesFile.readlines()
	unsatFilesList  = unsatFile.readlines() # nothing more to do, each line contains exactly one file
	
	solversFile.close()
	filesFile.close()
	unsatFile.close()

	print "checking unsat files ..."
	unsatFiles = set()
	for unsatHidoku in unsatFilesList:
		unsatFiles.add(unsatHidoku[:-1])	
	# print "unsat files: " + str(unsatFiles)
	
# split file data into filename and timeout
	
	files = list()
	timeouts = list()
		
	print "checking files ..."
	for line in fileData:
		blankPos = line.find(" ")
		fileName = line[0 : blankPos ]
		
		if 0 == os.path.exists(fileName) :
			print "file " + fileName + " does not exist or cannot be accessed, drop it"
			continue
		
		timeoutString = line [blankPos + 1:]
		timeout = DEFAULT_TIMEOUT
		if( is_number(timeoutString) ):
			timeout = int( timeoutString )
			
		files.append(fileName)
		timeouts.append(timeout)
		# print "found file " + fileName + " with timeout " + str(timeout)
	
# split solver data into solvername and solver call
	
	solverNames  = list() # name of the solver
	solverCalls  = list() # string to call the solver
	solverTime   = list() # time that has been used for solved instances
	solverSolved = list() # number of correctly solved instances
	solverFailed = list() # number of incorrect solutions
	
	
	print "parsing solvers ..."
	for line in solverData:
		blankPos = line.find(" ")
		solverName = line[0 : blankPos ]
		solverCall = line [blankPos + 1:-1]

		solverNames.append(solverName)
		solverCalls.append(solverCall)
		solverTime.append(0)
		solverSolved.append(0)
		solverFailed.append(0)
		# print "found file " + fileName + " with timeout " + str(timeout)
	
	print "default timeout: " +str(DEFAULT_TIMEOUT)
	print "default memory limit: " +str(GLOBALMEMORYLIMIT)
	
	print "run competition for " + str(len(solverNames)) + " solvers and " + str(len(files)) + " files\n\n"
	
	evaFile = open ("evaluation.dat","w")
	
	evaFile.write("instance ");
	sys.stdout.write("instance ");
	
	# delete old files!
	for solver in solverNames:
		evaFile.write(solver + " ");
		sys.stdout.write(solver + " ")
	evaFile.write("\n");
	sys.stdout.write("\n")
	

	# iterate over files
	for j in range(0, len(files) ):
		filename = files[j]
		basename = os.path.basename(filename)
		timeout  = timeouts[j]

		evaFile.write(filename + " ")
		sys.stdout.write(filename + "-(" + str(timeout) + "s)"+ " ");

		# iterate over solvers	
		for i in range(0, len(solverNames) ):
			solver = solverNames[i]
			call   = solverCalls[i] 

			# run the solver
			errFile = open("tmp/"+solver+"-"+basename+".err", "w");
			outFile = open("/tmp/"+solver+"-"+basename+"-"+str(pid), "w");
			os.chdir("solvers/" + solver)
			childTimer = os.times()[2]
			
			#solvingProcess = subprocess.Popen( str(call + " " +  filename).split(), shell=True, stderr=PIPE, stdout=PIPE, preexec_fn=signal.pause) # setlimits(timeout) ) #.communicate()[0]
			GLOBALTIMEOUT = timeout
			exitCode = subprocess.call(str(call + " " +  filename).split(), stdout=outFile, stderr=errFile, preexec_fn=setlimits )
			errFile.close()
			outFile.close()
			
			# wait for the solver to terminate
			childTimer = os.times()[2] - childTimer
			os.chdir(competitionDir)
			
			# debug output
			#	print "exit code: " + str(exitCode)
			#	print "file is unsat? file: " + filename + " unsat: " + str(unsatFiles) + " isIn " + str( filename in unsatFiles )
			
			# check UNSAT first
			if filename in unsatFiles:
				# print "found file " + filename + " in unsat files"
				if exitCode == 20:
					# recognized UNSAT correctly
					solverTime[i]   = solverTime[i]   + childTimer
					solverSolved[i] = solverSolved[i] + 1
					evaFile.write(str(childTimer) + " ")
					sys.stdout.write(str(childTimer) + " ");
					# solve next hidoku (consider next solver)
					continue

			
			
			# check output of solver whether it contains an solution
			wrongFileSize = os.path.getsize(filename) > os.path.getsize( "/tmp/"+solver+"-"+basename+"-"+str(pid) )
			
			#print wrongFileSize
			
			# if exit code is negative, the process has been terminated by a signal (including timeout)
			
			if childTimer >= timeout or exitCode != 10 or wrongFileSize == True :
				if childTimer >= timeout:
					evaFile.write("T ");
					sys.stdout.write("T ");
				else:
					if wrongFileSize:
						evaFile.write("Efile(" + str(os.path.getsize(filename)) + "-" + str(os.path.getsize( "/tmp/"+solver+"-"+basename+"-"+str(pid) )) + ")-E" + str(exitCode) + " ");
						sys.stdout.write("Efile ");
					else:
						evaFile.write("E" + str(exitCode) + " ");
						sys.stdout.write("E" + str(exitCode) + " ");

			else:
				# otherwise, check the returned solution
				exitCode = subprocess.call(["java", "Verifier", "/tmp/"+solver+"-"+basename+"-"+str(pid), filename], stdout=open('/dev/null', 'w'), stderr=open('/dev/null', 'w') )
	
				# evaluate the run
				if exitCode == 0:
					solverTime[i]   = solverTime[i]   + childTimer
					solverSolved[i] = solverSolved[i] + 1
					evaFile.write(str(childTimer) + " ")
					sys.stdout.write(str(childTimer) + " ");
					
				else:
					solverFailed[i] = solverFailed[i] + 1
					# copy faulty file!
					outFile = open("/tmp/"+solver+"-"+basename+"-"+str(pid), "r");
					errFile = open("tmp/"+solver+"-"+basename+".fail", "w+");
					for line in outFile.readlines():
						errFile.write( line )
					errFile.close()
					outFile.close()
					evaFile.write("F"+str(exitCode)+" ")
					sys.stdout.write("F"+str(exitCode)+"-" + str(childTimer) + " ");
					
		
		evaFile.write("\n")
		sys.stdout.write("\n");
		
#		print "CPU limit of parent (pid %d) after startup of child" % os.getpid(), resource.getrlimit(resource.RLIMIT_CPU)

		
		if j % 5 == 0:
			evaFile.close()
			subprocess.call(["./plotCactus.sh", "evaluation.dat", str( len(solverNames) ), "visualization"], stdout=open('/dev/null', 'w'), stderr=open('/dev/null', 'w') )
			evaFile = open ("evaluation.dat", "a" )

# do the final plot after all solvers have been executed
	evaFile.close()
	subprocess.call(["./plotCactus.sh", "evaluation.dat", str( len(solverNames) ), "visualization"], stdout=open('/dev/null', 'w'), stderr=open('/dev/null', 'w') )
	
	
	print "\n\nSolver Summary"
	for i in range(0, len(solverNames) ):
		solver = solverNames[i]
		solveT = solverTime[i] 
		solved = solverSolved[i]
		failed = solverFailed[i]
		print "[" + str(i) + "]" + solver + " :\t" + str(failed) + "\t " + str(solved) + "\t " + str(solveT)
	
# calculate the ranking
	for i in range(0, len(solverNames) ):

		for j in range(i+1, len(solverNames) ):
		
			solverI = solverNames[i]
			solveTI = solverTime[i] 
			solvedI = solverSolved[i]
			failedI = solverFailed[i]
		
			solverJ = solverNames[j]
			solveTJ = solverTime[j] 
			solvedJ = solverSolved[j]
			failedJ = solverFailed[j]
			
			# if i has more failed elements, or I solved less, or I is slower
			if  ( failedJ < failedI or
				  ( failedJ == failedI and solvedJ > solvedI ) or
				  ( failedJ == failedI and solvedJ == solvedI and solveTJ < solveTI) ):
						# swap elements in the lists
						solverNames[i], solverNames[j]   = solverNames[j], solverNames[i]
						solverTime[i] , solverTime[j]    = solverTime[j], solverTime[i]
						solverSolved[i], solverSolved[j] = solverSolved[j], solverSolved[i]
						solverFailed[i], solverFailed[j] = solverFailed[j], solverFailed[i]
	
	
# print the summary:
	print "\n\nSolver Ranking"
	for i in range(0, len(solverNames) ):
		solver = solverNames[i]
		solveT = solverTime[i] 
		solved = solverSolved[i]
		failed = solverFailed[i]
		print "[" + str(i) + "]" + solver + " :\t" + str(failed) + "\t " + str(solved) + "\t " + str(solveT)
	
	
# end of main-method
	
#
#	
# main method	
#
#

print "hidoku competition system"

if( len(sys.argv) < 3 ):
	print "usage: competition.py solver.list files.list unsatInstances.list"
	print "solver.list contains the call to the solvers"
	print "files.list  contains the list of files that should be handled, and a timeout for this file (separated by a blank)"
	print "unsatInstances.list contains the list of the unsatisfiable instances"
	print "Note: if no time out is specified, " + str(DEFAULT_TIMEOUT) + " seconds are assumed"
	exit(0)


solverList = ""
fileList = ""
unsatInstancesList = ""

if (len(sys.argv) > 1):
	solverList = sys.argv[1]

if (len(sys.argv) > 2):
	fileList = sys.argv[2]

if (len(sys.argv) > 2):
	unsatInstancesList = sys.argv[2]

main( solverList, fileList, unsatInstancesList );
