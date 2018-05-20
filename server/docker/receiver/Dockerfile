FROM debian:stretch
MAINTAINER JLR
RUN apt-get update

RUN apt-get install -y \
  make \
  g++ \
  libpqxx-dev \
  supervisor

RUN mkdir /opt/bbd

COPY docker/receiver/docker_assets/supervisord.conf /opt/bbd/supervisord.conf
COPY docker/receiver/docker_assets/supervisor_confs/ /opt/bbd/supervisor.conf.d/

ADD src/receiver /tmp/daemon
RUN make -C /tmp/daemon bbd_daemon
RUN make -C /tmp/daemon install

expose 7777

CMD supervisord -c /opt/bbd/supervisord.conf && tail -f /dev/null
