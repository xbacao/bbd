from django.shortcuts import render, redirect
from .models import Device, Valve, Schedule
from django.forms.formsets import formset_factory
from .forms import CicleForm, BaseCicleFormSet
import datetime
from rest_framework.decorators import api_view
from rest_framework.response import Response
from rest_framework import status
from django.db import connection

def bbd_index(request):
	context={}
	return render(request, 'bbd/bbd_index.html',context)

def devices_index(request):
	cursor = connection.cursor()
	cursor.execute('SELECT * FROM bbd.web_get_devices()')
	device_list=cursor.fetchall()
	context={'device_list': device_list}
	return render(request, 'bbd/devices_index.html',context)

def valves_index(request):
	cursor = connection.cursor()
	deviceid=request.GET.get('deviceid')
	if deviceid:
		cursor.execute('SELECT * FROM bbd.web_get_valves('+deviceid+')')
	else:
		cursor.execute('SELECT * FROM bbd.web_get_valves()')
	valve_list = cursor.fetchall()
	context = {'valve_list': valve_list}
	return render(request, 'bbd/valves_index.html',context)

def schedules_index(request):
	valveid=request.GET.get('valveid')
	cursor = connection.cursor()
	if valveid:
		cursor.execute('SELECT * FROM bbd.web_get_schedules('+valveid+')')
	else:
		cursor.execute('SELECT * FROM bbd.web_get_schedules()')
	schedule_list = cursor.fetchall()
	context = {'schedule_list': schedule_list}
	if valveid:
		context.update({'valveid':valveid})
	return render(request, 'bbd/schedules_index.html',context)

def add_schedule(request):
	valveid=request.GET.get('valveid')
	if valveid:
		context = {'valveid':valveid}
	else:
		context={}
	return render(request, 'bbd/add_schedule.html',context)

@api_view(['POST'])
def create_new_schedule(request):
	valveID_f=int(request.POST.get('valveID_f'))
	description=str(request.POST.get('description'))
	start_time=int(request.POST.get('start_h'))*60+int(request.POST.get('start_m'))
	stop_time=int(request.POST.get('stop_h'))*60+int(request.POST.get('stop_m'))

	valve_obj = Valve.objects.get(valveid=valveID_f)

	if start_time >=0 and start_time <= 1440 and stop_time >=0 \
	and stop_time <= 1440:
		schedule_obj=Schedule(valveid_f=valve_obj, description=description, \
		start_time=start_time, stop_time=stop_time, sent=False, active=True)

		schedule_obj.save()

		return Response(status=status.HTTP_200_OK)
	else:
		return Response(status=status.HTTP_400_BAD_REQUEST)

@api_view(['GET'])
def get_schedule(request):
	schedule_id=request.GET.get('schedule_id')
	if schedule_id:
		cursor = connection.cursor()
		cursor.execute('SELECT * FROM bbd.schedule WHERE scheduleID='+schedule_id)
		schedule=cursor.fetchone()
		data={'schedule_id':schedule[0], \
					'valve_id':schedule[1], \
					'description':schedule[2], \
					'created_on':schedule[3], \
					'start_time':schedule[4], \
					'stop_time':schedule[5], \
					'sent':str(schedule[6]), \
					'sent_on':schedule[7], \
					'active':str(schedule[8]), \
					'active_start':schedule[9], \
					'active_end':schedule[10]}
		return Response(data, status=status.HTTP_200_OK)
	else:
		return Response(status=status.HTTP_400_BAD_REQUEST)
