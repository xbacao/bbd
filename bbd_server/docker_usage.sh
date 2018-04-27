#!/bin/bash
S_PATH=`dirname $(readlink -f $0)`

RED='\033[0;31m'
NC='\033[0m'


function exit_on_error {
  printf "${RED}$1${NC}\n"
  exit "${2:-1}"
}

function usage {
  printf "usage: sh $0 [option]\n"
  printf "options:  -b   build bbd_server docker\n"
  printf "          -r   run bbd_server docker\n"
  printf "          -a   attach to bbd_server docker\n"
  printf "          -d   delete bbd_server docker\n"
}

function build {
  docker build -t bbd_server:latest . || exit_on_error "error building docker image!"
}

function delete {
  docker rm bbd_server || exit_on_error || "error deleting docker container!"
  #docker rmi -f bbd_server:latest || exit_on_error "error deleting docker images!"
}

function run {
  docker run -it --name bbd_server -p 80:80 bbd_server || exit_on_error "error running docker image!"
}

function attach {
  docker attach --name bbd_server || exit_on_error "error attaching to docker container!"
}

if [ "$#" -ne 1 ]; then
  usage
  exit_on_error "bad amount of arguments!"
fi

while getopts brsd arg; do
  case $arg in
    b)
    build
    ;;
    d)
    delete
    ;;
    r)
    run
    ;;
    *)
      usage
      exit_on_error "bad argument!"
      ;;
  esac
done
