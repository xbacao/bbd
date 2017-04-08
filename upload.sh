#!/bin/bash
S_PATH=`dirname $(readlink -f $0)`
SERVER_IP=178.62.6.44
SERVER_USER=f0lk
# SERVER_IP=192.168.81.63
# SERVER_USER=jribeiro
PROJECT_DIR=/opt/bbd_server
SERVER_FILES="bbd_server/daemon bbd_server/mySQL bbd_server/web_server bbd_server/logs bbd_server/run"
DAEMON_FILES="bbd_server/daemon bbd_server/logs bbd_server/run"
WEB_FILES="bbd_server/web_server"

if [ $# -ne 1 ]; then
  OP=PRINT_HELP
else
  case $1 in
      -d|--daemon)
      OP=SERVER_DAEMON
      ;;
      -s|--server-full)
      OP=SERVER_FULL
      ;;
      -w|--deploy-web)
      OP=SERVER_DEPLOY_WEB
      ;;
      -ls|--server-login)
      OP=SERVER_LOGIN
      ;;
      *)
      OP=PRINT_HELP
      ;;
  esac
fi

case ${OP} in
  SERVER_DAEMON)
  DAEMON_FILES_PATH=""
  for i in ${DAEMON_FILES}
  do
    DAEMON_FILES_PATH="${DAEMON_FILES_PATH} ${S_PATH}/${i} "
  done
  scp -r ${DAEMON_FILES_PATH} ${SERVER_USER}@${SERVER_IP}:${PROJECT_DIR}
  ;;
  SERVER_FULL)
  SERVER_FILES_PATH=""
  for i in ${SERVER_FILES}
  do
    SERVER_FILES_PATH="${SERVER_FILES_PATH} ${S_PATH}/${i} "
  done
  scp -r ${SERVER_FILES_PATH} ${SERVER_USER}@${SERVER_IP}:${PROJECT_DIR}
  ;;
  SERVER_DEPLOY_WEB)
  WEB_FILES_PATH=""
  for i in ${WEB_FILES}
  do
    WEB_FILES_PATH="${WEB_FILES_PATH} ${S_PATH}/${i}"
  done
  scp -r ${WEB_FILES_PATH} ${SERVER_USER}@${SERVER_IP}:${PROJECT_DIR}/web_server
  ;;
  SERVER_LOGIN)
  ssh ${SERVER_USER}@${SERVER_IP}
  ;;
  PRINT_HELP)
  echo "commands: ./upload.sh [option]"
  echo "options:"
  echo " -s              --server-full        uploads all of the server code to the server"
  echo " -d              --daemon             uploads the server c++ daemon code to the server"
  echo " -2              --deploy-web         deploys the web server code to the server"
  echo " -ls             --server-login       logs in to the server using ssh"
  ;;
esac
