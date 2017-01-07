from django.conf.urls import url

from . import views

urlpatterns = [
    url(r'^$', views.valves_index, name='valves_index'),
    url(r'^schedules/$', views.schedules_index, name='schedules_index'),
    url(r'^add_schedule/$', views.add_schedule, name='add_schedule'),

]