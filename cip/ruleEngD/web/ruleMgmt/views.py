from flask import Blueprint, request, redirect, render_template, url_for
from flask.views import MethodView
from ruleMgmt.models import Rule

rule = Blueprint('rule',__name__, template_folder = "templates")

class RuleView(MethodView):

    def get(self,cp):
        rule = Rule.objects.get_or_404(check_point = cp)
        return render_template('rule.html', rule = rule)

rule.add_url_rule('/<cp>/', view_func = RuleView.as_view('cp'))
