faces[f_counter].vert[v_counter++] = vert.position;

if (v_counter > 2) {
	++f_counter;
	v_counter = 0;
}


DirectX::XMFLOAT3 p0 = vect[0].position;
std::vector<DirectX::XMFLOAT3> norms(vect.size() - 2);
for (int i = 0; i < f_counter; ++i) {
	DirectX::XMFLOAT3 p1 = faces[i].vert[0];
	DirectX::XMFLOAT3 p2 = faces[i].vert[1];
	DirectX::XMFLOAT3 u = { p1.x - p0.x, p1.y - p0.y, p1.z - p0.z };
	DirectX::XMFLOAT3 vn = { p2.x - p0.x, p2.y - p0.y, p2.z - p0.z };

	DirectX::XMStoreFloat3(
		&faces[i].normal,
		DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&u), DirectX::XMLoadFloat3(&vn))
	);

	DirectX::XMStoreFloat3(
		&faces[i].normal,
		DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&faces[i].normal))
	);

}


{LOG_DURATION("Vert normals")
for (auto& vert : vect) {
	for (int i = 0; i < f_counter; ++i) {
		if (DirectX::XMVector3Equal(DirectX::XMLoadFloat3(&faces[i].vert[0]), DirectX::XMLoadFloat3(&vert.position))) {
			faces_indexes[adj_face++] = index;
			++index;
		}
		else if (DirectX::XMVector3Equal(DirectX::XMLoadFloat3(&faces[i].vert[1]), DirectX::XMLoadFloat3(&vert.position))) {
			faces_indexes[adj_face++] = index;
			++index;
		}
		else if (DirectX::XMVector3Equal(DirectX::XMLoadFloat3(&faces[i].vert[2]), DirectX::XMLoadFloat3(&vert.position))) {
			faces_indexes[adj_face++] = index;
			++index;
		}
	}

	for (int i = 0; i < adj_face; ++i) {
		f_index = faces_indexes[i];
		vert.normal.x += faces[f_index].normal.x;
		vert.normal.y += faces[f_index].normal.y;
		vert.normal.z += faces[f_index].normal.z;
	}
	vert.normal.x /= adj_face;
	vert.normal.y /= adj_face;
	vert.normal.z /= adj_face;

	//DirectX::XMStoreFloat3(
	//	&vert.normal,
	//	DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vert.normal))
	//);

	adj_face = 0;
	index = 0;
	f_index = 0;
}}