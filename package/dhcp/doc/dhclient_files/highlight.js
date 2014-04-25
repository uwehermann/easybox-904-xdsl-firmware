function highlightnode(r, z, n) {
 var i;
 for (i = 0; i < n.childNodes.length; i++) {
  var c = n.childNodes[i];
  if (c.nodeType == 3) {
   var j;
   while ((m = r.exec(c.data))) {
    var a = document.createTextNode(c.data.substr(m.index + m[0].length));
    var s = document.createElement("span");
    s.className = z;
    s.appendChild(document.createTextNode(m[0]));
    c.data = c.data.substr(0, m.index);
    if (i == (n.childNodes.length - 1))
     n.appendChild(a);
    else
     n.insertBefore(a, n.childNodes[i+1]);
    n.insertBefore(s, a);
    c = a;
    i += 2;
   }
  } else if (c.childNodes.length && typeof c.htmlFor == "undefined") {
   highlightnode(r, z, c);
  }
 }
}
function highlight() {
 var q;
 if (!document.createElement || !(q=document.referrer.match(/[?&][pq]=([^&]+)/))) return;
 var e = document.getElementById('content');
 if (!e) return;
 var w = unescape(q[1]).toLowerCase().match(/([a-z0-9_:.]{2,})/g);
 var i;
 for (i=0; i<w.length && i<10; i++) {
  highlightnode(new RegExp("\\b"+w[i]+"s?\\b", "i"), "highlight", e);
 }
}
if (window.addEventListener) {
 window.addEventListener('load', highlight, false); 
} else if (window.attachEvent) { 
 window.attachEvent('onload', highlight);
}
