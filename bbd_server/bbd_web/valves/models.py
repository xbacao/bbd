from __future__ import unicode_literals

from django.db import models


class Schedule(models.Model):
    scheduleid = models.AutoField(db_column='scheduleID', primary_key=True)  # Field name made lowercase.
    valveid_f = models.ForeignKey('Valve', models.DO_NOTHING, db_column='valveID_f')  # Field name made lowercase.
    description = models.CharField(max_length=100, blank=True, null=True)
    created_on = models.DateTimeField()
    active = models.IntegerField()
    active_start = models.DateTimeField(blank=True, null=True)
    active_end = models.DateTimeField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'schedule'


class ScheduleEntry(models.Model):
    entryid = models.AutoField(db_column='entryID', primary_key=True)  # Field name made lowercase.
    scheduleid_f = models.ForeignKey(Schedule, models.DO_NOTHING, db_column='scheduleID_f')  # Field name made lowercase.
    start_time = models.TimeField()
    stop_time = models.TimeField()

    class Meta:
        managed = False
        db_table = 'schedule_entry'


class Valve(models.Model):
    valveid = models.IntegerField(db_column='valveID', primary_key=True)  # Field name made lowercase.
    description = models.CharField(max_length=100, blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'valve'
