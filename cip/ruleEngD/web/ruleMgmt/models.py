import datetime
from flask import url_for
from ruleMgmt import db


class Rule(db.Document):
    create_time = db.DateTimeField(default=datetime.datetime.now, required=True)
    check_point = db.StringField(max_length=255, required=False)
    rule_name = db.StringField(max_length=255, required=False)
    owner = db.StringField(max_length=255, required=False)
    content = db.StringField(max_length=1024, required=False)

    def __str__(self):
        return self.check_point


