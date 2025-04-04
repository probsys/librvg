FROM        ubuntu:24.04

#           Copy the repository.
ADD         ./ /librvg

#           Build the library.
WORKDIR     /librvg
RUN         apt update && apt upgrade -y
RUN         apt install -y build-essential libgsl-dev libgmp-dev
RUN         make

#           Run the examples.
WORKDIR     /librvg/examples
RUN         make
RUN         ./main.out

#           Run the experiments (~20 minutes).
WORKDIR     /librvg/experiments
RUN         apt install -y python3-matplotlib python3-pandas python3-numpy
RUN         make table1
RUN         make table2
RUN         make figure8

#           Rebuild the documentation.
WORKDIR     /librvg
RUN         apt install -y pandoc
RUN         make api.html readme.html

ENTRYPOINT  ["/bin/bash"]
