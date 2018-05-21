from django.shortcuts import render, redirect
from .models import Device, Valve, Schedule
from django.forms.formsets import formset_factory
from .forms import CicleForm, BaseCicleFormSet
import datetime
from rest_framework.decorators import api_view
from rest_framework.response import Response
from rest_framework import status

def bbd_index(request):
	context={}
	return render(request, 'bbd/bbd_index.html',context)

def devices_index(request):
	context={'device_list': Device.objects.all()}
	return render(request, 'bbd/devices_index.html',context)

def valves_index(request):
	valve_list = Valve.objects.order_by('-valveid')
	context = {
		'valve_list': valve_list,
	}
	if request.method == 'POST':
		output_msg=""
		keys = request.POST.keys()
		if 'edit_valve' in keys:
			valve_obj=Valve.objects.get(valveid=request.POST.get('valve_id'))
			context.update({'valve_edit':valve_obj.valveid})
			output_msg="Valve edited succesfully"
		elif 'remove_valve' in keys:
			valve_obj=Valve.objects.get(valveid=request.POST.get('valve_id'))
			valve_obj.delete()
			output_msg="Valve deleted succesfully"
		elif 'commit' in keys:
			valve_obj=Valve.objects.get(valveid=request.POST.get('valve_id'))
			valve_obj.valveid=request.POST.get('valve_new_id')
			valve_obj.description=request.POST.get('description')
			valve_obj.save()
			output_msg="Valve edited succesfully"
		elif 'add_valve_btn' in keys:
			context.update({'new_valve_flag':1})
		elif 'new_valve_commit' in keys:
			valve_obj=Valve(valveid=request.POST.get('new_valve_id'),description=request.POST.get('new_valve_description'))
			valve_obj.save()
			output_msg="Valve created succesfully"
		context.update({'output_msg':output_msg})
	return render(request, 'bbd/valves_index.html',context)

def schedules_index(request):
	valve_list = Valve.objects.order_by('-valveid')
	context = {
		'valve_list': valve_list,
	}
	if request.method == 'POST':
		output_msg=""
		keys = request.POST.keys()
		if 'select_valve' in keys:
			request.session['valve_id']=request.POST.get('select_valve')
		elif 'add_new' in keys:
			request.session['valve_id']=request.POST.get('valve_id')
			return redirect('add_schedule')
		elif 'view_sche_btn' in keys:
			request.session['sche_id']=request.POST.get('sche_id')

	if 'valve_id' in request.session:
		valve_id = request.session.get('valve_id','')
		schedules = Schedule.objects.filter(valveid_f=valve_id)
		context.update({'schedule_list':schedules, 'valve_id':valve_id})

	if 'sche_id' in request.session:
	 	sche_obj = Schedule.objects.get(scheduleid=request.session.get('sche_id',''))
	 	context.update({'sche':sche_obj})
	return render(request, 'bbd/schedules_index.html',context)

def add_schedule(request):
	context = {'valve_id':request.session.get('valve_id','')}
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
