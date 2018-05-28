# This is an auto-generated Django model module.
# You'll have to do the following manually to clean this up:
#   * Rearrange models' order
#   * Make sure each model has one field with primary_key=True
#   * Make sure each ForeignKey has `on_delete` set to the desired behavior.
#   * Remove `managed = False` lines if you wish to allow Django to create, modify, and delete the table
# Feel free to rename the models, but don't rename db_table values or field names.
from __future__ import unicode_literals

from django.db import models
from datetime import datetime


class Device(models.Model):
    deviceid = models.IntegerField(primary_key=True)
    description = models.CharField(max_length=100, blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'device'

class Schedule(models.Model):
    scheduleid = models.AutoField(primary_key=True)
    valveid_f = models.ForeignKey('Valve', models.DO_NOTHING, db_column='valveid_f')
    description = models.CharField(max_length=100, blank=True, null=True)
    created_on = models.DateTimeField(default=datetime.now, blank=True)
    start_time = models.SmallIntegerField()
    stop_time = models.SmallIntegerField()
    sent = models.BooleanField(default=False)
    sent_on = models.DateTimeField(blank=True, null=True)
    active = models.BooleanField(default=True)
    deactivated_on = models.DateTimeField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'schedule'


class Valve(models.Model):
    valveid = models.IntegerField(primary_key=True)
    description = models.CharField(max_length=100, blank=True, null=True)
    deviceid_f = models.ForeignKey(Device, models.DO_NOTHING, db_column='deviceid_f')

    class Meta:
        managed = False
        db_table = 'valve'
