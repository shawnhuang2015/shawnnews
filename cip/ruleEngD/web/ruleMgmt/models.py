import datetime
from flask import url_for
from ruleMgmt import db

# Schema denormalization should satisfy below scenarios:
# 1. Show version list for single rule [ Rule.versions]
# 2. Show rule summary list for single checkpoint[ Checkpoint.rules ]
# 3. Show summary list for all rules [ Rule.meta ]
# 4. Rule Detail [ Rule.content ]

class RuleMeta(db.EmbeddedDocument):
    id = db.IntField()
    name = db.StringField(max_length=255, required=False)
    checkpoint = db.StringField(max_length=255, required=False)
    author = db.StringField(max_length=255, required=False)
    version = db.IntField(required = False)
    priority = db.IntField(required = False)
    create_time = db.DateTimeField(default=datetime.datetime.now, required=False)
    archived = db.BooleanField(default = False, required = False)

    def __str__(self):
        content = {
                "id": self.id,
                "name": self.name,
                "checkpoint": self.checkpoint,
                "author": self.author,
                "versoin": self.version,
                "priority": self.priority,
                "created_time": self.create_time,
                "archived":self.archived
                }
        return str(content)

class Rule(db.Document):
    # Note that 'meta' is keyword in mongoengine
    ruleMeta = db.EmbeddedDocumentField(RuleMeta)
    content = db.StringField(max_length=1024, required=False)
    versions = db.ListField(RuleMeta)

    def __str__(self):
        content = {
                "meta": self.ruleMeta,
                "content": self.content,
                "versions": self.versions
                }
        return str(content)


class Checkpoint(db.Document):
    name = db.StringField(max_length=255, required=False)
    rules = db.ListField(RuleMeta)

    def __str__(self):
        content = "name:%s" %self.name
        return content.join(rule in rules)
