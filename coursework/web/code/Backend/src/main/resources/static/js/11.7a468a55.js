(window["webpackJsonp"]=window["webpackJsonp"]||[]).push([[11],{cbUd:function(t,n,o){},hWJA:function(t,n,o){"use strict";var e=o("cbUd"),a=o.n(e);a.a},ulXf:function(t,n,o){"use strict";o.r(n);var e=function(){var t=this,n=t.$createElement,o=t._self._c||n;return o("div",{staticClass:"animated fadeIn"},[o("b-card",{attrs:{"no-body":""}},[o("b-card-header",[o("strong",[t._v("信息管理")])]),o("b-card-body",[o("b-row",{staticClass:"line-control"},[o("b-col",[t._v("学号: "+t._s(t.studentId))]),o("b-col",[t._v("姓名: "+t._s(t.studentName))])],1),o("b-row",{staticClass:"line-control"},[o("b-col",[t._v("性别: "+t._s(t.sex))]),o("b-col",[t._v("年龄: "+t._s(t.age))])],1),o("b-row",{staticClass:"line-control"},[o("b-col",[t._v("已修学分: "+t._s(t.credit))]),o("b-col",[t._v("GPA: "+t._s(t.gpa.toFixed(3)))])],1),o("b-row",{staticClass:"line-control"},[o("b-col",[t._v("班级: "+t._s(t.majorName+t.classNumber))]),o("b-col",[t._v("生源地: "+t._s(t.province+t.city))])],1),o("b-button",{attrs:{variant:"primary"},on:{click:t.goChangePassword}},[t._v("修改密码")])],1)],1)],1)},a=[],l=(o("VRzm"),o("vDqi")),r={name:"StudentInfo",methods:{goChangePassword:function(){this.$router.push("/changePassword")}},data:function(){return{studentId:null,studentName:null,credit:null,sex:null,age:null,gpa:null,classNumber:null,majorName:null,city:null,province:null}},mounted:function(){var t=this;l({method:"get",url:"/controller/getStudentInfo"}).then(function(n){if("ILLEGAL"!==n.data)for(var o in n.data)n.data.hasOwnProperty(o)&&(t[o]=n.data[o]);else t.$notify({group:"notify",type:"error",title:"错误",text:"非法操作, 请重新登录后再试!"})}).catch(function(n){console.log(n),t.$notify({group:"notify",type:"error",title:"错误",text:"网络错误!"})})}},s=r,c=(o("hWJA"),o("KHd+")),i=Object(c["a"])(s,e,a,!1,null,"422dbc14",null);n["default"]=i.exports}}]);
//# sourceMappingURL=11.7a468a55.js.map