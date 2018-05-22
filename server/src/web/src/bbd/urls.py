from django.conf.urls import url

from . import views

urlpatterns = [
    url(r'^$', views.bbd_index, name='bbd_index'),
    url(r'^devices/$', views.devices_index, name='devices_index'),
    url(r'^valves/$', views.valves_index, name='valves_index'),
    url(r'^schedules/$', views.schedules_index, name='schedules_index'),
    url(r'^add_schedule/$', views.add_schedule, name='add_schedule'),
    url(r'^create_new_schedule/$', views.create_new_schedule, name='create_new_schedule'),
    url(r'^get_schedule/$', views.get_schedule, name='get_schedule'),
]
