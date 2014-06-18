#
# build execution environment
#


# create required directories and builds the verifier
all: Verifier.class
	mkdir -p solvers tmp
	touch files.list solvers.list

# compile the verifier
Verifier.class: Verifier.java
	javac Verifier.java
	
# remove the verifier class file, and all other created files, except the solvers
clean:
	rm -f Verifier.class evaluation.dat visualization.eps visualization.pdf
	rm -rf tmp/*
	rm -f files.list solvers.list

