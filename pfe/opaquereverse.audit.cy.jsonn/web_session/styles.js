var styles = [ {
  "format_version" : "1.0",
  "generated_by" : "cytoscape-3.9.1",
  "target_cytoscapejs_version" : "~2.1",
  "title" : "default",
  "style" : [ {
    "selector" : "node",
    "css" : {
      "border-opacity" : 1.0,
      "shape" : "roundrectangle",
      "font-family" : "SansSerif.plain",
      "font-weight" : "normal",
      "width" : 75.0,
      "text-valign" : "center",
      "text-halign" : "center",
      "background-color" : "rgb(200,0,0)",
      "height" : 35.0,
      "background-opacity" : 1.0,
      "border-color" : "rgb(204,204,204)",
      "color" : "rgb(0,0,0)",
      "font-size" : 12,
      "border-width" : 0.0,
      "text-opacity" : 1.0,
      "content" : "data(label)"
    }
  }, {
    "selector" : "node[group > 24]",
    "css" : {
      "background-color" : "rgb(68,1,84)"
    }
  }, {
    "selector" : "node[group = 24]",
    "css" : {
      "background-color" : "rgb(31,225,237)"
    }
  }, {
    "selector" : "node[group > 18][group < 24]",
    "css" : {
      "background-color" : "mapData(group,18,24,rgb(223,52,52),rgb(31,225,237))"
    }
  }, {
    "selector" : "node[group > 12][group < 18]",
    "css" : {
      "background-color" : "mapData(group,12,18,rgb(137,105,200),rgb(223,52,52))"
    }
  }, {
    "selector" : "node[group > 6][group < 12]",
    "css" : {
      "background-color" : "mapData(group,6,12,rgb(25,167,172),rgb(137,105,200))"
    }
  }, {
    "selector" : "node[group > 0][group < 6]",
    "css" : {
      "background-color" : "mapData(group,0,6,rgb(173,220,48),rgb(25,167,172))"
    }
  }, {
    "selector" : "node[group = 0]",
    "css" : {
      "background-color" : "rgb(173,220,48)"
    }
  }, {
    "selector" : "node[group < 0]",
    "css" : {
      "background-color" : "rgb(253,231,37)"
    }
  }, {
    "selector" : "node[ id = '397' ]",
    "css" : { }
  }, {
    "selector" : "node:selected",
    "css" : {
      "background-color" : "rgb(255,255,0)"
    }
  }, {
    "selector" : "edge",
    "css" : {
      "opacity" : 1.0,
      "text-opacity" : 1.0,
      "font-size" : 10,
      "width" : 2.0,
      "source-arrow-shape" : "none",
      "line-color" : "rgb(132,132,132)",
      "source-arrow-color" : "rgb(0,0,0)",
      "target-arrow-color" : "rgb(0,0,0)",
      "line-style" : "dashed",
      "target-arrow-shape" : "triangle",
      "font-family" : "Dialog.plain",
      "font-weight" : "normal",
      "color" : "rgb(0,0,0)",
      "content" : "data(label)"
    }
  }, {
    "selector" : "edge:selected",
    "css" : {
      "line-color" : "rgb(255,0,0)"
    }
  } ]
} ]