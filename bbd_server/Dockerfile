FROM debian:latest
MAINTAINER JLR
RUN apt-get update
RUN apt-get install -y \
  postgresql \
  postgresql-client \
  libpqxx-dev \
  apache2-dev \
  python-virtualenv \
  python-dev\
  python3-dev \
  apache2 \
  python-pip \
  python-psycopg2 \
  libapache2-mod-wsgi \
  w3m \
  pkg-config \
  supervisor

RUN mkdir /opt/bbd
ADD daemon /tmp/daemon
RUN make -C /tmp/daemon bbd_daemon
RUN make -C /tmp/daemon install

#database
ENV POSTGRES_DB bbd
ENV POSTGRES_USER root
ENV POSTGRES_PASSWORD morcao123
COPY database/bbd_postgres.sql /tmp/db.sql
RUN service postgresql start && \
  su -c "psql -c \"SET TIME ZONE 'Europe/Lisbon'\"" postgres && \
  su -c "psql -c \"CREATE USER ${POSTGRES_USER} PASSWORD '${POSTGRES_PASSWORD}'\"" postgres && \
  su -c "psql -c \"CREATE DATABASE ${POSTGRES_DB}\"" postgres && \
  su -c "psql -c \"ALTER DATABASE ${POSTGRES_DB} OWNER TO ${POSTGRES_USER}\"" postgres && \
  psql -U $POSTGRES_USER -d ${POSTGRES_DB} -f /tmp/db.sql && \
  service postgresql stop

#web server
RUN mkdir -p /opt/bbd/web_server/src /opt/bbd/web_server/venv \
  /tmp/web_server_install
ADD web_server/src /opt/bbd/web_server/src
COPY web_server/install/apache2.conf /etc/apache2/apache2.conf
COPY web_server/install/requirements.txt /tmp/requirements.text
RUN pip install -r /tmp/requirements.text
RUN service postgresql start && \
 /usr/bin/python2 /opt/bbd/web_server/src/manage.py makemigrations && \
 /usr/bin/python2 /opt/bbd/web_server/src/manage.py migrate && \
 service postgresql stop

#supervisord
COPY docker_assets/supervisord.conf /opt/bbd/supervisord.conf
COPY docker_assets/supervisor_confs/ /opt/bbd/supervisor.conf.d/

expose 80 443 7777

CMD supervisord -c /opt/bbd/supervisord.conf && tail -f /dev/null
