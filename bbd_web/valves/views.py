from django.shortcuts import render
from .models import Valve, Schedule
from django.forms.formsets import formset_factory
from .forms import CicleForm, BaseCicleFormSet
import datetime

# Create your views here.
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
	return render(request, 'valves/valves_index.html',context)	

def schedules_index(request):
	valve_list = Valve.objects.order_by('-valveid')
	context = {
		'valve_list': valve_list,
	}
	if request.method == 'POST':
		output_msg=""
		keys = request.POST.keys()
		if 'select_valve' in keys:
			schedules = Schedule.objects.filter(valveid_f=request.POST.get('select_valve'))
			context.update({'schedule_list':schedules})
		context.update({'output_msg':output_msg})
	return render(request, 'valves/schedules_index.html',context)	

def add_schedule(request):
	context = {}
	cicleFormSet = formset_factory(CicleForm, formset=BaseCicleFormSet)
	valve_id_list = [x.valveid for x in Valve.objects.all()]

	cicle_data=[]
	
	if request.method == 'POST':
		output_msg=""
		keys = request.POST.keys()
		cicle_form_set=cicleFormSet(request.POST)

		if cicle_form_set.is_valid():

			for cicle_form in cicle_form_set:
				start_hour=cicle_form.cleaned_data.get('start_hour')
				start_min=cicle_form.cleaned_data.get('start_min')
				stop_hour=cicle_form.cleaned_data.get('stop_hour')
				stop_min=cicle_form.cleaned_data.get('stop_min')

				cicle_data.append({'start_hour':start_hour, 'start_min':start_min, 'stop_hour':stop_hour, 'stop_min':stop_min})
		if 'add_cicle' in keys:
			cicle_data.append([])
		context.update({'output_msg':output_msg})
	else:
		cicle_form_set=cicleFormSet(initial=cicle_data)
	context.update({'valve_id_list':valve_id_list,'cicle_form_set':cicle_form_set})
	return render(request, 'valves/add_schedule.html',context)