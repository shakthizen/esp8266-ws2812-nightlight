(window["webpackJsonp"]=window["webpackJsonp"]||[]).push([[4],{"8b24":function(t,n,e){"use strict";e.r(n);var i=function(){var t=this,n=t.$createElement,e=t._self._c||n;return e("q-page",{staticClass:"q-pa-md"},[e("div",{staticClass:"row q-pt-md"},[e("div",{staticClass:"col"},[e("q-select",{attrs:{options:t.fxEffects,label:"Effect",outlined:"","dropdown-icon":t.mdiMenuDown},on:{input:function(n){return t.setEffect()}},model:{value:t.fxEffect,callback:function(n){t.fxEffect=n},expression:"fxEffect"}})],1)]),e("div",{staticClass:"row q-pt-md"},[e("div",{staticClass:"col"},[e("q-select",{attrs:{options:t.fxAnimations,label:"Animation",outlined:"","dropdown-icon":t.mdiMenuDown},on:{input:function(n){return t.setEffect()}},model:{value:t.fxAnimation,callback:function(n){t.fxAnimation=n},expression:"fxAnimation"}})],1)]),e("div",{staticClass:"row q-pt-md"},[e("div",{staticClass:"col flex flex-center q-gutter-md"},[e("q-btn",{attrs:{stack:"",round:"",size:"lg",outline:"",color:"blue",icon:t.mdiChevronDoubleLeft},on:{click:function(n){return t.setColor(-10)}}}),e("q-btn",{attrs:{stack:"",round:"",size:"lg",outline:"",color:"blue",icon:t.mdiChevronLeft},on:{click:function(n){return t.setColor(-1)}}}),e("q-btn",{attrs:{stack:"",round:"",size:"lg",outline:"",color:"blue",icon:t.mdiChevronRight},on:{click:function(n){return t.setColor(1)}}}),e("q-btn",{attrs:{stack:"",round:"",size:"lg",outline:"",color:"blue",icon:t.mdiChevronDoubleRight},on:{click:function(n){return t.setColor(10)}}})],1)]),e("q-inner-loading",{attrs:{showing:t.connectionFailed,dark:""}},[e("q-spinner-gears",{attrs:{size:"50px",color:"white"}})],1),e("q-page-sticky",{attrs:{position:"bottom-right",offset:[18,18]}},[e("q-btn",{attrs:{fab:"",icon:t.mdiReload,color:"blue"},on:{click:function(n){return t.fetchStatus()}}})],1)],1)},o=[],a=e("5218"),s={name:"Index",data:()=>({mdiChevronDoubleLeft:a["a"],mdiChevronLeft:a["c"],mdiChevronRight:a["d"],mdiChevronDoubleRight:a["b"],mdiReload:a["f"],mdiMenuDown:a["e"],fxEffects:["White","Color","Gradient","Gradient (Forward Animation)","Gradient (Backward Animation)","Rainbow","Rainbow (Forward Animation)","Rainbow (Backward Animation)"],fxEffect:"",fxAnimations:["None","Glitter","Breathing","Wipe Forward (Slow)","Wipe Backward (Slow)","Wipe Forward (Fast)","Wipe Backward (Fast)","Chaser Forward","Chaser Backward","Meteor Forward","Meteor Backward","Sinelon","Random Fade","Fire","Fire Colored"],fxAnimation:"",fxValue:0,overlay:!1,connectionFailed:!1}),computed:{fxEffectIndex:function(){return this.fxEffects.indexOf(this.fxEffect)},fxAnimationIndex:function(){return this.fxAnimations.indexOf(this.fxAnimation)}},methods:{fetchStatus:function(){this.overlay=!0,this.$axios.get("/api/status").then((t=>{let n=t.data.split(",");this.fxEffect=this.fxEffects[n[0]],this.fxAnimation=this.fxAnimations[n[1]],this.fxValue=n[2],this.overlay=!1})).catch((()=>{this.overlay=!0,this.connectionFailed=!0}))},setEffect:function(){this.overlay=!0,this.$axios.get(`/api/set?fxeffect=${this.fxEffectIndex}&fxanimation=${this.fxAnimationIndex}`).then((()=>{this.overlay=!1})).catch((()=>{this.overlay=!0,this.connectionFailed=!0}))},setColor:function(t){this.overlay=!0,this.$axios.get(`/api/set?fxcolor=${t}`).then((()=>{this.overlay=!1})).catch((()=>{this.overlay=!0,this.connectionFailed=!0}))}},created:function(){this.fetchStatus()}},c=s,r=e("2877"),l=e("9989"),f=e("ddd8"),d=e("9c40"),u=e("74f7"),h=e("cf57"),m=e("de5e"),x=e("eebe"),p=e.n(x),w=Object(r["a"])(c,i,o,!1,null,null,null);n["default"]=w.exports;p()(w,"components",{QPage:l["a"],QSelect:f["a"],QBtn:d["a"],QInnerLoading:u["a"],QSpinnerGears:h["a"],QPageSticky:m["a"]})}}]);