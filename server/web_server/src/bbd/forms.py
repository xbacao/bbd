from django import forms
from django.forms.formsets import BaseFormSet

class CicleForm(forms.Form):
    """
    Form for individual user links
    """
    start_hour = forms.IntegerField(
                    max_value=24,
                    min_value=0,
                    widget=forms.TextInput(attrs={
                        'placeholder': 'start hour',
                    }),
                    required=True)
    start_min = forms.IntegerField(
                    max_value=60,
                    min_value=0,
                    widget=forms.TextInput(attrs={
                        'placeholder': 'start minute',
                    }),
                    required=True)
    stop_hour = forms.IntegerField(
                    max_value=24,
                    min_value=0,
                    widget=forms.TextInput(attrs={
                        'placeholder': 'stop hour',
                    }),
                    required=True)
    stop_min = forms.IntegerField(
                    max_value=60,
                    min_value=0,
                    widget=forms.TextInput(attrs={
                        'placeholder': 'stop min',
                    }),
                    required=True)


class BaseCicleFormSet(BaseFormSet):
    def clean(self):
        """
        Adds validation to check that no two links have the same anchor or URL
        and that all links have both an anchor and URL.
        """
        if any(self.errors):
            return

        start_hour = []
        start_min = []
        stop_hour = []
        stop_min = []

        for form in self.forms:
            if form.cleaned_data:
                start_hour = int(form.cleaned_data['start_hour'])
                start_min = int(form.cleaned_data['start_min'])
                stop_hour = int(form.cleaned_data['stop_hour'])
                stop_min = int(form.cleaned_data['stop_min'])

                # Check that all links have both an anchor and URL
                if not start_hour or not start_min or not stop_hour or not stop_min:
                    raise forms.ValidationError(
                        'All links must have an anchor.',
                        code='missing_anchor'
                    )

                if start_hour > 23 or start_hour < 0:
                    raise forms.ValidationError(
                        'Invalid start hour.',
                        code='inv_s_hour'
                    )

                if start_min > 59 or start_min < 0:
                    raise forms.ValidationError(
                        'Invalid start min.',
                        code='inv_s_min'
                    )

                if stop_hour > 23 or stop_hour < 0:
                    raise forms.ValidationError(
                        'Invalid stop hour.',
                        code='inv_hour'
                    )

                if stop_min > 23 or stop_min < 0:
                    raise forms.ValidationError(
                        'Invalid stop min.',
                        code='inv_min'
                    )