from django.shortcuts import render, redirect
from .models import Device, Valve, Schedule
from django.forms.formsets import formset_factory
from .forms import CicleForm, BaseCicleFormSet
from rest_framework.decorators import api_view
from rest_framework.response import Response
from rest_framework import status
from django.db import connection
from django.contrib import messages
import datetime

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
	if request.method == "POST":
		sel_schedule_id=request.POST.get('sel_schedule_id')
		if sel_schedule_id:
			cursor = connection.cursor()
			cursor.execute('SELECT * FROM bbd.schedule WHERE scheduleID='+sel_schedule_id)
			schedule_sel=cursor.fetchone()
			context.update({'selected_sche':{'schedule_id':schedule_sel[0], \
						'valve_id':schedule_sel[1], \
						'description':schedule_sel[2], \
						'created_on':schedule_sel[3], \
						'start_time':'%02d:%02d' % (schedule_sel[4]/60, schedule_sel[4]%60), \
						'stop_time':'%02d:%02d' % (schedule_sel[5]/60, schedule_sel[5]%60), \
						'sent':schedule_sel[6], \
						'sent_on':schedule_sel[7], \
						'active':schedule_sel[8], \
						'deactivated_on':schedule_sel[9]}})
	return render(request, 'bbd/schedules_index.html',context)

def add_schedule(request):
	context = {}
	valveid=request.GET.get('valveid')
	if valveid:
		context.update({'valveid':valveid})
	cursor=connection.cursor()
	cursor.execute('SELECT valveID from bbd.valve')
	valve_id_list=[x[0] for x in cursor.fetchall()]
	context.update({'valve_id_list':valve_id_list})
	return render(request, 'bbd/add_schedule.html',context)

@api_view(['POST'])
def create_new_schedule(request):
	valveID_f=int(request.POST.get('valveid_f'))
	description=str(request.POST.get('description'))
	start_time=int(request.POST.get('start_h'))*60+int(request.POST.get('start_m'))
	stop_time=int(request.POST.get('stop_h'))*60+int(request.POST.get('stop_m'))

	valve_obj = Valve.objects.get(valveid=valveID_f)

	if start_time >=0 and start_time <= 1440 and stop_time >=0 \
	and stop_time <= 1440:
		cursor = connection.cursor()
		cursor.execute('SELECT FROM bbd.web_create_new_schedule('\
								+str(valveID_f)+','\
								+'\''+description+'\','\
								+str(start_time)+'::smallint,'\
								+str(stop_time)+'::smallint)')
		messages.success(request, 'schedule added succesfuly')
		return redirect(schedules_index)
	else:
		return Response(status=status.HTTP_400_BAD_REQUEST)

@api_view(['POST'])
def deactivate_schedule(request):
	schedule_id=request.POST.get('schedule_id')
	if schedule_id:
		cursor = connection.cursor()
		cursor.execute('SELECT web_deactivate_schedule_request('+schedule_id+')')
		messages.success(request, 'schedule deactivated succesfuly')
		return redirect(schedules_index)
	else:
		return Response(status=status.HTTP_400_BAD_REQUEST)
