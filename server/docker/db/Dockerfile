FROM debian:stretch
MAINTAINER JLR

RUN apt-get update
RUN apt-get install -y \
  postgresql \
  supervisor

RUN mkdir /opt/bbd

#database
ENV POSTGRES_DB bbd
ENV POSTGRES_USER root
ENV POSTGRES_PASSWORD morcao123
COPY src/db/bbd.sql /tmp/db.sql
RUN service postgresql start && \
  su -c "psql -c \"SET TIME ZONE 'Europe/Lisbon'\"" postgres && \
  su -c "psql -c \"CREATE USER ${POSTGRES_USER} PASSWORD '${POSTGRES_PASSWORD}'\"" postgres && \
  su -c "psql -c \"CREATE DATABASE ${POSTGRES_DB}\"" postgres && \
  su -c "psql -c \"ALTER DATABASE ${POSTGRES_DB} OWNER TO ${POSTGRES_USER}\"" postgres && \
  psql -U $POSTGRES_USER -d ${POSTGRES_DB} -f /tmp/db.sql && \
  service postgresql stop

#supervisord
COPY docker/db/docker_assets/supervisord.conf /opt/bbd/supervisord.conf
COPY docker/db/docker_assets/supervisor.conf.d/ /opt/bbd/supervisor.conf.d/
COPY docker/db/docker_assets/postgresql.conf /etc/postgresql/9.6/main/postgresql.conf
COPY docker/db/docker_assets/pg_hba.conf /etc/postgresql/9.6/main/pg_hba.conf

expose 5432

CMD supervisord -c /opt/bbd/supervisord.conf && tail -f /dev/null
