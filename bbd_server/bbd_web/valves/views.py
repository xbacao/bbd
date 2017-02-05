from django.shortcuts import render, redirect
from .models import Valve, Schedule, ScheduleEntry
from django.forms.formsets import formset_factory
from .forms import CicleForm, BaseCicleFormSet
import datetime

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
			valve_id=request.POST.get('select_valve')
			schedules = Schedule.objects.filter(valveid_f=valve_id)
			context.update({'schedule_list':schedules, 'valve_id':valve_id})
		elif 'add_new' in keys:
			request.session['valve_id']=request.POST.get('valve_id')
			return redirect('add_schedule')
		elif 'view_sche_btn' in keys:
			request.session['sche_id']=request.POST.get('sche_id')
			return redirect('view_schedule')
		context.update({'output_msg':output_msg})
	return render(request, 'valves/schedules_index.html',context)

def add_schedule(request):
	context = {}
	cicleFormSet = formset_factory(CicleForm, formset=BaseCicleFormSet)
	valve_id=request.session.get('valve_id','')

	if request.method == 'POST':
		output_msg=""
		keys=request.POST.keys()

		if 'submit_sche' in keys:
			cicle_form_set_o = cicleFormSet(request.POST)
			if cicle_form_set_o.is_valid():

				#create sche
				valve_obj = Valve.objects.get(valveid=valve_id)
				schedule_obj=Schedule(valveid_f=valve_obj, description=request.POST.get('description'))

				schedule_obj.save()

				for cicle_form in cicle_form_set_o:
					start_time = str(cicle_form.cleaned_data.get('start_hour'))+':'+str(cicle_form.cleaned_data.get('start_min'))+':00'
					stop_time = str(cicle_form.cleaned_data.get('stop_hour'))+':'+str(cicle_form.cleaned_data.get('stop_min'))+':00'
					sche_entry_obj = ScheduleEntry(scheduleid_f=schedule_obj, start_time=start_time, stop_time=stop_time)
					sche_entry_obj.save()



		context.update({'output_msg':output_msg})
	cicle_form_set=cicleFormSet(initial=[])
	context.update({'valve_id':valve_id,'cicle_form_set':cicle_form_set})
	return render(request, 'valves/add_schedule.html',context)

def view_schedule(request):
	sche_id = request.session.get('sche_id','')
	sche_obj = Schedule.objects.get(scheduleid=sche_id)
	entry_objs = ScheduleEntry.objects.filter(scheduleid_f=sche_obj)
	context={'sche':sche_obj, 'entries':entry_objs}
	return render(request, 'valves/view_schedule.html',context)
