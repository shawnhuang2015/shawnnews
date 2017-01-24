from flask import Blueprint, request, redirect, render_template, url_for
from flask.views import MethodView
from ruleMgmt.models import Rule


class ViewRuleDetail(MethodView):

    def get(self,rule_id):
        targetRule = Rule.objects.get_or_404(ruleMeta__id = rule_id)
        return render_template('ruleDetail.html', rule = targetRule)

class ViewVersions(MethodView):

    def get(self, rule_id):
        targetRule = Rule.objects.get_or_404(ruleMeta__id = rule_id)
        return render_template('versions.html', versions = targetRule.versions)

class ViewRuleList(MethodView):

    def get(self):
        ruleCollection = Rule.objects.all()
        return render_template('ruleList.html', rules = ruleCollection)

    def get(self, checkpoint):
        ruleCollection = Rule.objects.get_or_404(ruleMeta__checkpoint = checkpoint)
        return render_template('ruleList.html', rules = ruleCollection)

controler = Blueprint('ruleMgmt',__name__, template_folder = "templates")
controler.add_url_rule('/details/<rule_id>/', view_func = ViewRuleDetail.as_view('ruleDetails'))
controler.add_url_rule('/versions/<rule_id>/', view_func = ViewVersions.as_view('ruleVersions'))
controler.add_url_rule('/', view_func = ViewRuleList.as_view('ruleList'))
