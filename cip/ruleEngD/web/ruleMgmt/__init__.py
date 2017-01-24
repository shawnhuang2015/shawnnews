from flask import Flask
app = Flask(__name__)

# Mongoengine
app.config["MONGODB_DB"] = "ruleMgmt"
app.config["SECRET_KEY"] = "KeepThisS3cr3t"

from flask.ext.mongoengine import MongoEngine
db = MongoEngine(app)

# View
from ruleMgmt.views import controler 
app.register_blueprint(controler)

if __name__ == "__main__":
    app.run()
