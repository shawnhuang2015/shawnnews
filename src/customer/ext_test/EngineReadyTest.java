import java.io.*;
import java.util.*;
import java.net.*;
import java.nio.charset.Charset;

import org.json.*;

public class EngineReadyTest {

	private static class GraphSqlRestReader {
		private String readAll(Reader rd) throws IOException {
			StringBuilder sb = new StringBuilder();
			int cp;
			while ((cp = rd.read()) != -1) {
				sb.append((char) cp);
			}
			return sb.toString();
		}

		public JSONObject readJsonFromUrl(String actiontype, String url,
				String postdata) {
			if (url.startsWith("dummy")) {
				try {
					JSONObject json = new JSONObject(postdata);
					System.out.println(actiontype + ": " + json.toString());
					return new JSONObject();
				} catch (Exception ex) {
					System.out.println(ex.toString());
					System.out.println(actiontype + ": " + postdata);
					System.exit(0);
				}
			}
			String jsonText = "";
			try {
				URL obj = new URL(url);
				HttpURLConnection con = (HttpURLConnection) obj
						.openConnection();
				con.setRequestMethod("POST");
				con.setRequestProperty("User-Agent", "GSQL");
				con.setRequestProperty("Content-Type", "application/json");
				con.setDoOutput(true);
				DataOutputStream wr = new DataOutputStream(
						con.getOutputStream());
				wr.writeBytes(postdata);
				wr.close();
				InputStream is = con.getInputStream();
				BufferedReader rd = new BufferedReader(new InputStreamReader(
						is, Charset.forName("UTF-8")));
				jsonText = readAll(rd);
				// System.out.println(jsonText);
				JSONObject json = new JSONObject(jsonText);
				is.close();
				rd.close();
				return json;
			} catch (Exception ex) {
				System.out.println(ex.toString());
				System.out.println(jsonText);
				System.out.println(url + ": " + actiontype + ": " + postdata);
				System.exit(0);
			}
			return new JSONObject();
		}
	}

	private static class Configuration {
		String url_;
		String mode_;
		int startround_;
		int endround_;
		int vertexsize_;
		int totalthreads_;
		int badrecord_seed_;
		int deleterecord_seed_;
		int updaterecord_seed_;

		public Configuration(String url, String mode, int startround,
				int endround, int totalthreads, int vertexsize,
				int badrecord_seed, int deleterecord_seed, int updaterecord_seed) {
			url_ = url;
			mode_ = mode;
			startround_ = startround;
			endround_ = endround;
			vertexsize_ = vertexsize;
			totalthreads_ = totalthreads;
			badrecord_seed_ = badrecord_seed;
			deleterecord_seed_ = deleterecord_seed;
			updaterecord_seed_ = updaterecord_seed;
		}
	}

	private static class QueryThread implements Runnable {
		Configuration config_;
		int stageindex_;
		int threadindex_;
		long ms_;
		GraphSqlRestReader reader_;
		Thread t_;
		Vector<String> matchededges_;

		public QueryThread(Configuration config, int stageindex, int threadindex) {
			config_ = config;
			stageindex_ = stageindex;
			threadindex_ = threadindex;
			ms_ = 0;
			reader_ = new GraphSqlRestReader();
			matchededges_ = new Vector<String>();
			t_ = new Thread(this);
			t_.start();
		}

		public void run() {
			if (stageindex_ == 1)
				return;
			long begintm = System.nanoTime();
			int beginvertexid = (stageindex_ - 2) * config_.vertexsize_;
			int endvertexid = beginvertexid + config_.vertexsize_;
			for (int id = beginvertexid + threadindex_; id < endvertexid - 1; id += config_.totalthreads_) {
				Query(id);
			}
			ms_ = (System.nanoTime() - begintm) / 1000000;
		}

		void Query(int id) {
			// Query(id, true);
			Query(id, false);
		}

		void Query(int id, Boolean usingudf) {
			Boolean badid = (id % config_.badrecord_seed_) == 0;
			Boolean deleteid = (id % config_.deleterecord_seed_) == 0;
			Boolean updateid = (id % config_.updaterecord_seed_) == 0;
			String vattr_postdata = "{\"function\":\"vattr"
					+ (usingudf ? "_udf" : "")
					+ "\",\"translate_typed_ids\": [{\"id\": \"v_t0_" + id
					+ "\",\"type\":0}]}";
			JSONObject vattjson = reader_.readJsonFromUrl("VAttr", config_.url_
					+ "/builtins", vattr_postdata);
			if (!config_.url_.startsWith("dummy")) {
				assert vattjson.getBoolean("error") == badid || deleteid : vattr_postdata
						+ "|" + vattjson.toString();
				if (!badid && !deleteid) {
					assert Integer.parseInt(vattjson.getJSONObject("results")
							.getJSONObject("attr").getString("v0_a1")) == (updateid ? id - 1
							: id) : vattr_postdata + "|" + vattjson.toString();
				}
			}
			String edge_postdata = "{\"function\":\"edges"
					+ (usingudf ? "_udf" : "")
					+ "\",\"translate_typed_ids\": [{\"id\": \"v_t0_" + id
					+ "\",\"type\":0}],\"edgeattr\":true,\"tgtvattr\":false}";
			JSONObject edgejson = reader_.readJsonFromUrl("Edge", config_.url_
					+ "/builtins", edge_postdata);
			if (!config_.url_.startsWith("dummy")) {
				assert edgejson.getBoolean("error") == (badid || deleteid) : edge_postdata
						+ "|" + edgejson.toString();
				if (!badid && !deleteid) {
					matchededges_.clear();
					if ((id + 1) % config_.vertexsize_ != 0)
						AddMatchEdge(id, id + 1, 0);
					if (id % config_.vertexsize_ != 0)
						AddMatchEdge(id, id - 1, 1);
					if ((id + 1) % config_.vertexsize_ != 0)
						AddMatchEdge(id, id + 1, 1);
					Collections.sort(matchededges_);
					String expectresult = "";
					for (int i = 0; i < matchededges_.size(); ++i)
						expectresult += matchededges_.get(i) + ",";

					JSONArray edgearray = edgejson.getJSONObject("results")
							.getJSONArray("edges");
					matchededges_.clear();
					for (int i = 0; i < edgearray.length(); ++i) {
						String edgetype = Integer.toString(edgearray
								.getJSONObject(i).getInt("type"));
						String toid = edgearray.getJSONObject(i).getString(
								"to_vid");
						String attr = edgearray
								.getJSONObject(i)
								.getJSONObject("edgeattr")
								.getString(
										edgetype.equals("0") ? "e0_a1"
												: "e1_a1");
						matchededges_.add(edgetype + "|" + toid + "|" + attr);
					}
					Collections.sort(matchededges_);
					String returnresult = "";
					for (int i = 0; i < matchededges_.size(); ++i)
						returnresult += matchededges_.get(i) + ",";
					assert expectresult.equals(returnresult) : edge_postdata
							+ "|" + expectresult + "|" + returnresult;
				}
			}
		}

		void AddMatchEdge(int id1, int id2, int type) {
			if ((id2) % config_.badrecord_seed_ == 0
					|| (id2) % config_.deleterecord_seed_ == 0
					|| (id1 + id2) % config_.badrecord_seed_ == 0
					|| (id1 + id2) % config_.deleterecord_seed_ == 0)
				return;
			Boolean isupdate = (id1 + id2) % config_.updaterecord_seed_ == 0;
			String attr = "";
			if (type == 0)
				attr = Integer.toString(isupdate ? (id1 + id2 - 1)
						: (id1 + id2));
			else
				attr = isupdate ? "false" : "true";
			matchededges_.add(type + "|v_t0_" + id2 + "|" + attr);
		}
	}

	private static class PostThread implements Runnable {
		Configuration config_;
		int stageindex_;
		int threadindex_;
		long ms_;
		GraphSqlRestReader reader_;
		Thread t_;

		public PostThread(Configuration config, int stageindex, int threadindex) {
			config_ = config;
			stageindex_ = stageindex;
			threadindex_ = threadindex;
			ms_ = 0;
			reader_ = new GraphSqlRestReader();
			t_ = new Thread(this);
			t_.start();
		}

		public void run() {
			long begintm = System.nanoTime();
			int beginvertexid = (stageindex_ - 1) * config_.vertexsize_;
			int endvertexid = beginvertexid + config_.vertexsize_;
			for (int id = beginvertexid + threadindex_; id < endvertexid - 1; id += config_.totalthreads_) {
				Insert(id, id + 1);
			}
			for (int id = beginvertexid + threadindex_; id < endvertexid - 1; id += config_.totalthreads_) {
				Update(id, id + 1);
			}
			for (int id = beginvertexid + threadindex_; id < endvertexid - 1; id += config_.totalthreads_) {
				Delete(id, id + 1);
			}
			ms_ = (System.nanoTime() - begintm) / 1000000;
		}

		private void Insert(int i1, int i2) {
			Boolean badid1 = (i1 % config_.badrecord_seed_) == 0;
			Boolean badid2 = (i2 % config_.badrecord_seed_) == 0;
			Boolean badedge = ((i1 + i2) % config_.badrecord_seed_) == 0;
			String postdata = "{\"nodeList\":[{\"id\":\"v_t0_" + i1
					+ "\",\"v0_a1\":" + i1
					+ (badid1 ? ",\"invalidFlag\":true" : "")
					+ "}, {\"id\":\"v_t0_" + i2 + "\",\"v0_a1\":" + i2
					+ (badid2 ? ",\"invalidFlag\":true" : "")
					+ "}],\"edgeList\":[{\"startNode\":\"v_t0_" + i1
					+ "\",\"endNode\":\"v_t0_" + i2
					+ "\",\"typeid\":0,\"e0_a1\":" + Integer.toString(i1 + i2)
					+ (badedge ? ",\"invalidFlag\":true" : "")
					+ "},{\"startNode\":\"v_t0_" + i1
					+ "\",\"endNode\":\"v_t0_" + i2
					+ "\",\"typeid\":1,\"e1_a1\":true"
					+ (badedge ? ",\"invalidFlag\":true" : "") + "}]}";

			JSONObject json = reader_.readJsonFromUrl("Insert", config_.url_
					+ "/updategraph", postdata);
			if (!config_.url_.startsWith("dummy")) {
				assert json.getString("errorCode").equals("0") : postdata + "|"
						+ json.toString();
				assert json.getJSONObject("context").getInt("edgeFailCount") == 0 : postdata
						+ "|" + json.toString();
				assert json.getJSONObject("context").getInt("edgeSubmitCount") == 2 : postdata
						+ "|" + json.toString();
				assert json.getJSONObject("context").getInt("nodeFailCount") == 0 : postdata
						+ "|" + json.toString();
				assert json.getJSONObject("context").getInt("nodeSubmitCount") == 2 : postdata
						+ "|" + json.toString();
			}
		}

		private void Update(int i1, int i2) {
			Boolean badid1 = (i1 % config_.badrecord_seed_) == 0;
			Boolean badid2 = (i2 % config_.badrecord_seed_) == 0;
			Boolean badedge = ((i1 + i2) % config_.badrecord_seed_) == 0;
			Boolean updateid1 = (i1 % config_.updaterecord_seed_) == 0;
			Boolean updateid2 = (i2 % config_.updaterecord_seed_) == 0;
			Boolean updateedge = ((i1 + i2) % config_.updaterecord_seed_) == 0;
			String postdata = "{\"nodeList\":[{\"id\":\"v_t0_" + i1
					+ "\",\"v0_a1\":" + (updateid1 ? (i1 - 1) : i1)
					+ (badid1 ? ",\"invalidFlag\":true" : "")
					+ "}, {\"id\":\"v_t0_" + i2 + "\",\"v0_a1\":"
					+ (updateid2 ? (i2 - 1) : i2)
					+ (badid2 ? ",\"invalidFlag\":true" : "")
					+ "}],\"edgeList\":[{\"startNode\":\"v_t0_" + i1
					+ "\",\"endNode\":\"v_t0_" + i2
					+ "\",\"typeid\":0,\"e0_a1\":"
					+ (updateedge ? (i1 + i2 - 1) : i1 + i2)
					+ (badedge ? ",\"invalidFlag\":true" : "")
					+ "},{\"startNode\":\"v_t0_" + i1
					+ "\",\"endNode\":\"v_t0_" + i2
					+ "\",\"typeid\":1,\"e1_a1\":"
					+ (updateedge ? "false" : "true")
					+ (badedge ? ",\"invalidFlag\":true" : "") + "}]}";
			JSONObject json = reader_.readJsonFromUrl("Update", config_.url_
					+ "/updategraph", postdata);
			if (!config_.url_.startsWith("dummy")) {
				assert json.getString("errorCode").equals("0") : postdata + "|"
						+ json.toString();
				assert json.getJSONObject("context").getInt("edgeFailCount") == 0 : postdata
						+ "|" + json.toString();
				assert json.getJSONObject("context").getInt("edgeSubmitCount") == 2 : postdata
						+ "|" + json.toString();
				assert json.getJSONObject("context").getInt("nodeFailCount") == 0 : postdata
						+ "|" + json.toString();
				assert json.getJSONObject("context").getInt("nodeSubmitCount") == 2 : postdata
						+ "|" + json.toString();
			}
		}

		private void Delete(int i1, int i2) {
			Boolean badid1 = (i1 % config_.badrecord_seed_) == 0;
			Boolean badid2 = (i2 % config_.badrecord_seed_) == 0;
			Boolean badedge = ((i1 + i2) % config_.badrecord_seed_) == 0;
			Boolean deleteid1 = (i1 % config_.deleterecord_seed_) == 0;
			Boolean deleteid2 = (i2 % config_.deleterecord_seed_) == 0;
			Boolean deleteedge = ((i1 + i2) % config_.deleterecord_seed_) == 0;
			if (!deleteid1 && !deleteid2 && !deleteedge)
				return;
			String postdata = "{\"nodeList\":[{\"id\":\"v_t0_" + i1 + "\""
					+ (deleteid1 ? ",\"deleteFlag\":true" : "")
					+ (badid1 ? ",\"invalidFlag\":true" : "")
					+ "}, {\"id\":\"v_t0_" + i2 + "\""
					+ (deleteid2 ? ",\"deleteFlag\":true" : "")
					+ (badid2 ? ",\"invalidFlag\":true" : "")
					+ "}],\"edgeList\":[{\"startNode\":\"v_t0_" + i1
					+ "\",\"endNode\":\"v_t0_" + i2 + "\",\"typeid\":0"
					+ (deleteedge ? ",\"deleteFlag\":true" : "")
					+ (badedge ? ",\"invalidFlag\":true" : "")
					+ "},{\"startNode\":\"v_t0_" + i1
					+ "\",\"endNode\":\"v_t0_" + i2 + "\",\"typeid\":1"
					+ (deleteedge ? ",\"deleteFlag\":true" : "")
					+ (badedge ? ",\"invalidFlag\":true" : "") + "}]}";
			JSONObject json = reader_.readJsonFromUrl("Delete", config_.url_
					+ "/updategraph", postdata);
			if (!config_.url_.startsWith("dummy")) {
				assert json.getString("errorCode").equals("0") : postdata + "|"
						+ json.toString();
				assert json.getJSONObject("context").getInt("edgeFailCount") == 0 : postdata
						+ "|" + json.toString();
				assert json.getJSONObject("context").getInt("edgeSubmitCount") == 2 : postdata
						+ "|" + json.toString();
				assert json.getJSONObject("context").getInt("nodeFailCount") == 0 : postdata
						+ "|" + json.toString();
				assert json.getJSONObject("context").getInt("nodeSubmitCount") == 2 : postdata
						+ "|" + json.toString();
			}
		}

	}

	public static void main(String[] args) throws Exception {
		Configuration config = new Configuration(args[0], args[1],
				Integer.parseInt(args[2]), Integer.parseInt(args[3]),
				Integer.parseInt(args[4]), Integer.parseInt(args[5]),
				Integer.parseInt(args[6]), Integer.parseInt(args[7]),
				Integer.parseInt(args[8]));
		for (int roundindex = config.startround_; roundindex <= config.endround_; ++roundindex) {
			Vector<PostThread> postthreads = new Vector<PostThread>();
			Vector<QueryThread> querythreads = new Vector<QueryThread>();
			if (config.mode_.indexOf("Post") >= 0) {
				for (int i = 0; i < config.totalthreads_; ++i) {
					postthreads.add(new PostThread(config, roundindex, i));
				}
			}
			if (config.mode_.indexOf("Query") >= 0) {
				for (int i = 0; i < config.totalthreads_; ++i) {
					querythreads.add(new QueryThread(config, roundindex, i));
				}
			}
			long totalpostms = 0;
			long totalqueryms = 0;
			for (int i = 0; i < postthreads.size(); ++i) {
				postthreads.get(i).t_.join();
				totalpostms += postthreads.get(i).ms_;
			}
			for (int i = 0; i < querythreads.size(); ++i) {
				querythreads.get(i).t_.join();
				totalqueryms += querythreads.get(i).ms_;
			}
			System.out.println(config.mode_ + " Round " + roundindex
					+ ": total post ms: " + totalpostms + ", total query ms: "
					+ totalqueryms);
		}
	}
}
