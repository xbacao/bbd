#!/bin/bash
S_PATH=`dirname $(readlink -f $0)`

RED='\033[0;31m'
NC='\033[0m'


function exit_on_error {
  printf "${RED}$1${NC}\n"
  exit "${2:-1}"
}

function usage {
  printf "usage: sh $0 [option] [component]\n"
  printf "options:  -b   build bbd_server docker\n"
  printf "          -r   run bbd_server docker\n"
  printf "          -a   attach to bbd_server docker\n"
  printf "          -d   delete bbd_server docker\n"
  printf "components: receiver  c/c++ receiver daemon\n"
  printf "            db        postgresql database\n"
  printf "            web       python/django web server\n"
}

function build {
  #docker build -t bbd_$1:latest docker/$1 || (rm $S_PATH/docker/$1/src_tmp; exit_on_error "error building docker image!")
  docker build -f docker/$1/Dockerfile -t bbd_$1:latest . || (exit_on_error "error building docker image!")
}

function delete {
  docker kill bbd_$1
  docker rm bbd_$1 || exit_on_error || "error deleting docker container!"
  #docker rmi -f bbd_server:latest || exit_on_error "error deleting docker images!"
}

function run {
  case $1 in
    receiver)
      #docker run -dit --cap-add=SYS_PTRACE --name bbd_$1 --hostname bbd_$1 -p 7777:7777 bbd_$1 || exit_on_error "error running docker image!"
      docker run -dit --name bbd_$1 --net=bbd_network -p 7777:7777 bbd_$1 || exit_on_error "error running docker image!"
      ;;
    db)
      docker run -dit --name bbd_$1 --net=bbd_network bbd_$1 || exit_on_error "error running docker image!"
      ;;
    web)
      docker run -dit --name bbd_$1 --net=bbd_network bbd_$1 -p 80:80 || exit_on_error "error running docker image!"
      ;;
    *)
      usage; exit 1
      ;;
  esac

}

function attach {
  docker exec -it bbd_$1 /bin/bash || exit_on_error "error attaching to docker container!"
}

if [ "$#" -ne 2 ]; then
  usage
  exit_on_error "bad amount of arguments!"
fi

case $2 in
  receiver);;
  db);;
  web);;
  *) usage; exit 1;;
esac

case $1 in
  -b)
  build $2
  ;;
  -d)
  delete $2
  ;;
  -r)
  run $2
  ;;
  -a)
  attach $2
  ;;
  *)
    usage
    exit_on_error "bad argument!"
    ;;
esac
