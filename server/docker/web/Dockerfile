FROM debian:stretch
MAINTAINER JLR
RUN apt-get update
RUN apt-get install -y \
  apache2-dev \
  python-dev\
  libpq-dev \
  apache2 \
  python-pip \
  libapache2-mod-wsgi \
  supervisor

RUN mkdir /opt/bbd

#web server
RUN mkdir -p /opt/bbd/web_server/src /opt/bbd/web_server/venv \
  /tmp/web_server_install

COPY src/web/install/requirements.txt /tmp/requirements.text
RUN pip install -r /tmp/requirements.text

COPY src/web/install/apache2.conf /etc/apache2/apache2.conf

#supervisord
COPY docker/web/docker_assets/supervisord.conf /opt/bbd/supervisord.conf
COPY docker/web/docker_assets/supervisor_confs/ /opt/bbd/supervisor.conf.d/

ADD src/web/src /opt/bbd/web_server/src

expose 80

CMD /usr/bin/python /opt/bbd/web_server/src/manage.py makemigrations;\
          /usr/bin/python /opt/bbd/web_server/src/manage.py migrate; \
          supervisord -c /opt/bbd/supervisord.conf && tail -f /dev/null
