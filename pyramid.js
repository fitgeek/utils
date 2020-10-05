var http = require('http');
var express = require('express');
var app = express();

function compare(a, b) {
  const A = a.num;
  const B = b.num;

  let comparison = 0;
  if (A > B) {
    comparison = 1;
  } else if (A < B) {
    comparison = -1;
  }
  return comparison;
  //return comparison * -1;
}

app.get('/:word', function(request, response) {
    var word = request.params.word ;
    var array = [];

    /* pyramid word */
    for (var i=0; i<word.length; i++) {
        var exists = 0;
        char = word[i];
        for (var j=0; j <array.length; j++) {
            var counter = array[j];
            if (char === counter.char) {
                counter.num += 1;
                array[j] = counter;
                exists = 1;
            }
        }
        if (!exists) {
            array.push({char:char, num:1});
        }
    }
    const list = array.sort(compare);

    /* test array */
    let testlist = list.map(x => x);
    let pyramid = "Pyramid";
    let lastnum = testlist.shift().num;
    for (var i=0; i<testlist.length; i++) {
        var num = testlist[i].num;
        if ((lastnum + 1) !== num) {
            pyramid = "Not a pyramid";
            break;
        }
        lastnum = num;
    }
    let results = {"pyramid": pyramid, "list": list};

    response.end(JSON.stringify(results));
});

var httpServer = http.createServer(app);
httpServer.listen(3000);
