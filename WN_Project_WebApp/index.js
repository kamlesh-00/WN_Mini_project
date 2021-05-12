//jshint esversion:6

const express = require("express");
const ejs = require("ejs");
const bodyParser = require("body-parser");

const app = express();

app.use(express.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(express.static("public"));
app.set("view engine", "ejs");

var bulb = "Off";
var fan = "Off";

app.get("/", (req, res) => {
  res.render("home", { bulb_value: bulb, fan_value: fan });
});

app.get("/api/getInfo", (req, res) => {
  res.status(200).json({ fan: fan, bulb: bulb });
});

app.post("/api/sendInfo", (req, res) => {
  if (req.body.bulb == "0") {
    bulb = "Off";
  } else if (req.body.bulb == "1") {
    bulb = "On";
  }
  if (req.body.fan == "0") {
    fan = "Off";
  } else if (req.body.fan == "1") {
    fan = "On";
  }
  console.log(req.body);
  res.redirect("/");
});

const port = process.env.PORT || 3000;
app.listen(port, () => {
  console.log("Server started at " + port);
});
